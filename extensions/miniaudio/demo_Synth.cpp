/*
	olc::PixelGameEngine3 Example - demo_Synth

	Demonstrates using synth callbacks from olcPGEX3_Miniaudio

	Licenced under the OLC-3 License
*/

#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

#define OLC_PGEX3_MINIAUDIO
#include "olcPGEX3_Miniaudio.h"

// Save us a whole lot of typing in this demo
using namespace olc::ext::Miniaudio;

constexpr int NOTE_COUNT = 17;
constexpr float thirtyFramesPerSecond = 1.0f / 30.f;

// ---------------------------------------------------------------------------
// Synth building blocks. Everything in here is called from the AUDIO THREAD,
// so: no allocation, no locks, no rand(), no I/O.
// ---------------------------------------------------------------------------
namespace synth
{
	constexpr double kPi    = 3.14159265358979323846;
	constexpr double kTwoPi = 2.0 * kPi;

	// Don't generate harmonics above this; anything past Nyquist (24 kHz at
	// 48 kHz sample rate) folds back down as harsh aliasing.
	constexpr double kMaxHarmonicHz = 20000.0;

	enum class Oscillator
	{
		SINE,
		SQUARE,
		TRIANGLE,
		SAW_ANALOG,
		SAW_DIGITAL,
		NOISE,
	};

	// Converts frequency (Hz) to angular velocity
	inline double w(const double hertz)
	{
		return hertz * kTwoPi;
	}

	// Pre-computed equal-tempered scale: 256 Hz * 2^(noteID / 12).
	// Avoids calling pow() several times per sample per note.
	constexpr int kScaleOffset = 24;
	constexpr int kScaleSize   = 128;

	inline const std::array<double, kScaleSize> kScaleTable = []
	{
		std::array<double, kScaleSize> table{};
		for(int i = 0; i < kScaleSize; i++)
			table[i] = 256.0 * std::pow(2.0, double(i - kScaleOffset) / 12.0);
		return table;
	}();

	inline double scale(const int noteID)
	{
		return kScaleTable[std::clamp(noteID + kScaleOffset, 0, kScaleSize - 1)];
	}

	// Tiny lock-free RNG (xorshift32). Only ever touched by the audio thread.
	inline uint32_t g_noiseState = 2463534242u;

	inline float noise()
	{
		g_noiseState ^= g_noiseState << 13;
		g_noiseState ^= g_noiseState >> 17;
		g_noiseState ^= g_noiseState << 5;
		return 2.0f * (float(g_noiseState) / 4294967295.0f) - 1.0f;
	}

	// PolyBLEP: smooths the discontinuity of a naive square/saw so it doesn't alias.
	// t = normalised phase [0,1), dt = phase increment per sample.
	inline double polyBlep(double t, const double dt)
	{
		if(t < dt)
		{
			t /= dt;
			return t + t - t * t - 1.0;
		}
		if(t > 1.0 - dt)
		{
			t = (t - 1.0) / dt;
			return t * t + t + t + 1.0;
		}
		return 0.0;
	}

	// Oscillator.
	//   time          absolute time in seconds (double!)
	//   dt            seconds per sample (used for band-limiting)
	//   hertz         frequency
	//   LFOHertz/LFOAmplitude  optional vibrato
	//   custom        max harmonic count for SAW_ANALOG
	inline double osc(const double time, const double dt, const double hertz,
		const Oscillator type = Oscillator::SINE,
		const double LFOHertz = 0.0, const double LFOAmplitude = 0.0,
		const double custom = 50.0)
	{
		double phase = w(hertz) * time;
		if(LFOAmplitude != 0.0)
			phase += LFOAmplitude * hertz * std::sin(w(LFOHertz) * time);

		// Phase increment per sample, as a fraction of a cycle (for BLEP)
		const double inc = std::min(hertz * dt, 0.49);

		switch(type)
		{
			case Oscillator::SINE: // Sine wave between -1 and +1
				return std::sin(phase);

			case Oscillator::SQUARE: // Band-limited square wave between -1 and +1
			{
				double p = phase / kTwoPi;
				p -= std::floor(p);
				double v = (p < 0.5) ? 1.0 : -1.0;
				v += polyBlep(p, inc);
				v -= polyBlep(std::fmod(p + 0.5, 1.0), inc);
				return v;
			}

			case Oscillator::TRIANGLE: // Triangle wave between -1 and +1
				return std::asin(std::sin(phase)) * (2.0 / kPi);

			case Oscillator::SAW_ANALOG: // Saw wave (analogue / warm)
			{
				// Sum of sin(n*x)/n, using the Chebyshev recurrence
				//   sin((n+1)x) = 2cos(x) sin(nx) - sin((n-1)x)
				// so we only need ONE sin and ONE cos instead of one sin per harmonic.
				int maxN = std::min(int(custom) - 1, int(kMaxHarmonicHz / hertz));
				if(maxN < 1) maxN = 1;

				const double c2 = 2.0 * std::cos(phase);
				double s0 = 0.0;
				double s1 = std::sin(phase);
				double out = s1;

				for(int n = 2; n <= maxN; n++)
				{
					const double s2 = c2 * s1 - s0;
					out += s2 / n;
					s0 = s1;
					s1 = s2;
				}
				return out * (2.0 / kPi);
			}

			case Oscillator::SAW_DIGITAL: // Band-limited digital saw
			{
				double p = phase / kTwoPi;
				p -= std::floor(p);
				return 2.0 * p - 1.0 - polyBlep(p, inc);
			}

			case Oscillator::NOISE:
				return noise();

			default:
				return 0.0;
		}
	}

	// ADSR parameters (just data; shared by every voice using an instrument)
	struct EnvelopeADSR
	{
		double attackTime       = 0.1;
		double decayTime        = 0.1;
		double sustainAmplitude = 1.0;
		double releaseTime      = 0.2;
		double startAmplitude   = 1.0;
	};

	// Per-voice envelope STATE. Generates one amplitude value per sample and
	// always continues from its current level, so retriggering a note that is
	// still fading out never causes a sudden jump (= click).
	struct EnvelopeState
	{
		enum class Stage { Idle, Attack, Decay, Sustain, Release };

		Stage  stage{Stage::Idle};
		double level{0.0};
		double releaseStep{0.0};

		bool active() const { return stage != Stage::Idle; }

		void noteOn()
		{
			stage = Stage::Attack; // level is deliberately NOT reset
		}

		void noteOff(const EnvelopeADSR& e, const double dt)
		{
			if(stage == Stage::Idle || stage == Stage::Release)
				return;
			stage = Stage::Release;
			// Fall linearly from wherever we are to zero over releaseTime
			releaseStep = level * dt / std::max(e.releaseTime, 1e-4);
		}

		double next(const EnvelopeADSR& e, const double dt)
		{
			constexpr double kMinStageTime = 1e-4;

			switch(stage)
			{
				case Stage::Idle:
					return 0.0;

				case Stage::Attack:
					level += dt * e.startAmplitude / std::max(e.attackTime, kMinStageTime);
					if(level >= e.startAmplitude)
					{
						level = e.startAmplitude;
						stage = Stage::Decay;
					}
					break;

				case Stage::Decay:
				{
					const double step = dt * std::abs(e.startAmplitude - e.sustainAmplitude)
						/ std::max(e.decayTime, kMinStageTime);

					if(level > e.sustainAmplitude)
					{
						level -= step;
						if(level <= e.sustainAmplitude)
						{
							level = e.sustainAmplitude;
							stage = Stage::Sustain;
						}
					}
					else
					{
						level += step;
						if(level >= e.sustainAmplitude)
						{
							level = e.sustainAmplitude;
							stage = Stage::Sustain;
						}
					}
					break;
				}

				case Stage::Sustain:
					level = e.sustainAmplitude;
					// A zero sustain (e.g. bell) is silent: stop doing work.
					if(level <= 0.0)
						stage = Stage::Idle;
					break;

				case Stage::Release:
					level -= releaseStep;
					if(level <= 0.0)
					{
						level = 0.0;
						stage = Stage::Idle;
					}
					break;
			}

			return level;
		}
	};

	// Abstract base for instruments. An instrument only produces the raw
	// waveform; the envelope and volume are applied by the voice.
	struct Instrument
	{
		virtual ~Instrument() = default;

		std::string  name;
		EnvelopeADSR env;
		float        gain{1.0f};

		virtual float sound(const double time, const double dt, const int noteId) const = 0;
	};

	struct Bell : public Instrument
	{
		Bell()
		{
			env.attackTime       = 0.01;
			env.decayTime        = 1.0;
			env.sustainAmplitude = 0.0;
			env.releaseTime      = 1.0;

			gain = 1.0f;
			name = "Bell";
		}

		float sound(const double time, const double dt, const int id) const override
		{
			return float(
				+ 1.00 * osc(time, dt, scale(id + 12), Oscillator::SINE, 5.0, 0.001)
				+ 0.50 * osc(time, dt, scale(id + 24))
				+ 0.25 * osc(time, dt, scale(id + 36)));
		}
	};

	struct Bell8 : public Instrument
	{
		Bell8()
		{
			env.attackTime       = 0.01;
			env.decayTime        = 0.5;
			env.sustainAmplitude = 0.8;
			env.releaseTime      = 1.0;

			gain = 1.0f;
			name = "Bell 8-bit";
		}

		float sound(const double time, const double dt, const int id) const override
		{
			return float(
				+ 1.00 * osc(time, dt, scale(id), Oscillator::SQUARE, 5.0, 0.001)
				+ 0.50 * osc(time, dt, scale(id + 12))
				+ 0.25 * osc(time, dt, scale(id + 24)));
		}
	};

	struct Harmonica : public Instrument
	{
		Harmonica()
		{
			env.attackTime       = 0.05;
			env.decayTime        = 1.0;
			env.sustainAmplitude = 0.95;
			env.releaseTime      = 0.1;

			gain = 1.0f;
			name = "Harmonica";
		}

		float sound(const double time, const double dt, const int id) const override
		{
			return float(
				+ 1.00 * osc(time, dt, scale(id),      Oscillator::SQUARE, 5.0, 0.001)
				+ 0.50 * osc(time, dt, scale(id + 12), Oscillator::SQUARE)
				+ 0.05 * osc(time, dt, scale(id + 24), Oscillator::NOISE));
		}
	};

	struct SawAnalog : public Instrument
	{
		SawAnalog()
		{
			env.attackTime       = 0.05;
			env.decayTime        = 1.0;
			env.sustainAmplitude = 0.95;
			env.releaseTime      = 0.1;

			gain = 1.0f;
			name = "Analog Saw";
		}

		float sound(const double time, const double dt, const int id) const override
		{
			return float(osc(time, dt, scale(id), Oscillator::SAW_ANALOG, 5.0, 0.001));
		}
	};

	// Everything the audio thread keeps for one playing note.
	// Touched ONLY by the audio thread.
	struct Voice
	{
		EnvelopeState     env;
		const Instrument* inst{nullptr}; // instrument this note started with
		bool              gate{false};   // last key state the audio thread saw
	};
}

class Example_Synth : public olc::PixelGameEngine
{
    // NOTICE
    //
    // This example is highly inspired by the Sound Synthesizer Part 3 series created by javidx9!
    // Checkout the original at https://github.com/OneLoneCoder/synth/blob/master/main3a.cpp
    // and the accompanying video at https://www.youtube.com/watch?v=kDuvruJTjOs
    //

public:
    Example_Synth()
    {
		sAppName = "Example - demo_Synth";
    	if(!InstallSystemExtension(&audio))
			throw std::runtime_error("Failed to install olcPGEX3_Miniaudio");
    }
    
public:

    bool OnUserCreate() override
    {
		// Create instruments BEFORE the audio callback is installed. After that
		// this list is read-only, so the audio thread can safely read it.
		instruments.emplace_back(std::make_unique<synth::Harmonica>());
		instruments.emplace_back(std::make_unique<synth::Bell>());
		instruments.emplace_back(std::make_unique<synth::Bell8>());
		instruments.emplace_back(std::make_unique<synth::SawAnalog>());

		sampleRate = double(audio.GetDeviceSampleRate());
		sampleDt   = 1.0 / sampleRate;

		// One-pole smoothing coefficient with a ~10 ms time constant
		smoothCoef = 1.0 - std::exp(-1.0 / (0.010 * sampleRate));
		smoothVolume = volume.load();
		smoothPan    = pan.load();

		// This is it... This is how we interface into the miniaudio engine any noise we want heard!
		// Called once per sample frame, on the AUDIO thread.
		noiseCallbackFunc = [this](float& noiseLeftChannel, float& noiseRightChannel, const float)->void
		{
			// Time comes from an integer sample counter: exact, never drifts,
			// and keeps full double precision no matter how long we run.
			const double time = double(sampleCounter++) * sampleDt;

			// Smooth the controls toward their targets (prevents zipper noise)
			smoothVolume += (double(volume.load(std::memory_order_relaxed)) - smoothVolume) * smoothCoef;
			smoothPan    += (double(pan.load(std::memory_order_relaxed))    - smoothPan)    * smoothCoef;

			const synth::Instrument* selected =
				instruments[selectedInstrumentInd.load(std::memory_order_relaxed)].get();

			double mix = 0.0;

			for(int i = 0; i < NOTE_COUNT; i++)
			{
				synth::Voice& v = voices[i];

				// Detect key press / release using the audio clock only
				const bool held = keyHeld[i].load(std::memory_order_relaxed);
				if(held != v.gate)
				{
					v.gate = held;

					if(held)
					{
						// Only adopt the newly selected instrument if this voice is
						// silent. If it's still fading out we keep its old instrument
						// so the waveform doesn't change abruptly mid-sound.
						if(!v.env.active())
							v.inst = selected;
						v.env.noteOn();
					}
					else if(v.inst)
					{
						v.env.noteOff(v.inst->env, sampleDt);
					}
				}

				// Silent voice: skip ALL oscillator math
				if(!v.env.active())
					continue;

				const double amp = v.env.next(v.inst->env, sampleDt);
				mix += amp * double(v.inst->sound(time, sampleDt, keys[i].id)) * double(v.inst->gain);
			}

			mix *= smoothVolume;

			// This lerp formula shrinks the -1 to 1 range to 0 to 1, which determines
			// the percentage of the total noise to play in each side.
			const double t = smoothPan * 0.5 + 0.5;

			// Soft clip. Keeps the summed notes below 1.0 so the header's limiter
			// (instant gain ducking = audible pumping/crackle) never has to act.
			noiseLeftChannel  = float(std::tanh(mix * (1.0 - t)));
			noiseRightChannel = float(std::tanh(mix * t));
		};

		// And finally... Tell the system this is where to send audio requests to... Now we can modify audio!
		audio.SetSynthCallback(noiseCallbackFunc);

        return true;
    }
    
    bool OnUserUpdate(float fElapsedTime) override
    {
        fElapsedTime = (fElapsedTime > thirtyFramesPerSecond) ? thirtyFramesPerSecond : fElapsedTime;
        
        olc::Pixel backgroundCol{olc::Colour::VERY_DARK_GREY};

		bool keyPressed{false};

		// Just publish which keys are held. The audio thread works out the
		// press/release edges and timing itself.
		for(int i = 0; i < NOTE_COUNT; i++)
		{
			const bool held = keyboard.GetKey(keys[i].key).bHeld;
			keyHeld[i].store(held, std::memory_order_relaxed);
			keyPressed |= held;
		}

		const int instrumentCount = int(instruments.size());
		int   instrumentInd = selectedInstrumentInd.load();
		float vol = volume.load();
		float pn  = pan.load();

		if(keyboard.GetKey(olc::Key::UP).bPressed)
			instrumentInd = (instrumentInd + 1) % instrumentCount;

		if(keyboard.GetKey(olc::Key::DOWN).bPressed)
			instrumentInd = (instrumentInd + instrumentCount - 1) % instrumentCount;

		if(keyboard.GetKey(olc::Key::RIGHT).bHeld)
			vol = std::min(1.f, vol + 1.f * fElapsedTime);
		if(keyboard.GetKey(olc::Key::LEFT).bHeld)
			vol = std::max(0.f, vol - 1.f * fElapsedTime);

		if(keyboard.GetKey(olc::Key::O).bHeld)
			pn = std::max(-1.f, pn - 1.f * fElapsedTime);
		if(keyboard.GetKey(olc::Key::P).bHeld)
			pn = std::min(1.f, pn + 1.f * fElapsedTime);

		if(keyboard.GetKey(olc::Key::R).bPressed)
		{
			instrumentInd = 0;
			vol = 0.1f;
			pn = 0.f;
		}

		selectedInstrumentInd.store(instrumentInd);
		volume.store(vol);
		pan.store(pn);

        if(keyPressed)
            backgroundCol = olc::Colour::VERY_DARK_BLUE;
		
        draw.Clear(backgroundCol);


		olc::vi2d center = ScreenSize() / 2;
		olc::vf2d scale{2.3f, 2.3f};
		std::string demoMessage = "olcPGEX3_miniaudio Waveform Demo";

		draw.String(
			center - olc::vi2d{0, 32} - (draw.GetTextSize(demoMessage, false, scale) / 2),
			demoMessage,
			olc::Colour::WHITE,
			scale
		);

		scale = {1.5f, 1.5f};
		demoMessage = "Instrument: <"+ instruments[instrumentInd]->name +"> UP, DOWN";
		draw.String(
			center - (draw.GetTextSize(demoMessage, false, scale) / 2),
			demoMessage,
			olc::Colour::WHITE,
			scale
		);

		demoMessage = "Volume: <"+ std::to_string(vol) +"> LEFT, RIGHT";
		draw.String(
			center + olc::vi2d{0, 24} - (draw.GetTextSize(demoMessage, false, scale) / 2),
			demoMessage,
			olc::Colour::WHITE,
			scale
		);

		demoMessage = "Pan: <"+ std::to_string(pn) +"> O, P";
		draw.String(
			center + olc::vi2d{0, 48} - (draw.GetTextSize(demoMessage, false, scale) / 2),
			demoMessage,
			olc::Colour::WHITE,
			scale
		);

		demoMessage = "Reset Settings <R>";
		draw.String(
			center + olc::vi2d{0, 72} - (draw.GetTextSize(demoMessage, false, scale) / 2),
			demoMessage,
			olc::Colour::WHITE,
			scale
		);

		olc::vf2d pianoStrSize{draw.GetTextSize(piano)};
        draw.String(
			olc::vf2d{
				(ScreenSize().x / 2) - (pianoStrSize.x / 2),
				ScreenSize().y - pianoStrSize.y
			},
			piano,
			olc::Colour::WHITE
		);

        #if defined(__EMSCRIPTEN__)
            return true;
        #else
            return !keyboard.GetKey(olc::Key::ESCAPE).bPressed;
        #endif
    }

private:

    // The instance of the audio engine, no fancy config required.
    AudioEngine audio;

	std::function<void(float& noiseLeftChannel, float& noiseRightChannel, const float fElapsedTime)> noiseCallbackFunc;

	// ---- Shared between threads: only simple atomics ----------------------
	std::atomic<float> volume{0.1f};
	std::atomic<float> pan{0.0f}; // -1.f for only left channel, 1.f for only right channel
	std::atomic<int>   selectedInstrumentInd{0};
	std::array<std::atomic<bool>, NOTE_COUNT> keyHeld{};

	// ---- Owned exclusively by the audio thread ----------------------------
	uint64_t sampleCounter{0}; // running count of samples generated
	double   sampleRate{48000.0};
	double   sampleDt{1.0 / 48000.0};
	double   smoothCoef{0.002};
	double   smoothVolume{0.1};
	double   smoothPan{0.0};
	std::array<synth::Voice, NOTE_COUNT> voices{};

	// Read-only once the audio callback is installed
	std::vector<std::unique_ptr<synth::Instrument>> instruments;

	struct KeyDef
	{
		const char* label; // display name of the note
		olc::Key    key;
		int         id;    // Position in scale
	};

	static inline const std::array<KeyDef, NOTE_COUNT> keys{{
		{"G#", olc::Key::A,		-1}, //This is not a typo. javid's original scale formula starts at "A", so this will retrieve the note ID prior to that one.
		{"A" , olc::Key::Z,		0},
		{"A#", olc::Key::S,		1},
		{"B" , olc::Key::X,		2},
		{"C" , olc::Key::C,		3},
		{"C#", olc::Key::F,		4},
		{"D" , olc::Key::V,		5},
		{"D#", olc::Key::G,		6},
		{"E" , olc::Key::B,		7},
		{"F" , olc::Key::N,		8},
		{"F#", olc::Key::J,		9},
		{"G" , olc::Key::M,		10},
		{"G#", olc::Key::K,		11},
		{"A" , olc::Key::COMMA,	12},
		{"A#", olc::Key::L,		13},
		{"B" , olc::Key::PERIOD,	14},
		{"C" , olc::Key::OEM_2,	15},
	}};

    const std::string piano{
	    "  | |   |   |   |   | |   |   |   |   | |   | |   |   |   |\n"
	    "A | | S |   |   | F | | G |   |   | J | | K | | L |   |   |\n"
	    "__| |___|   |   |___| |___|   |   |___| |___| |___|   |   |__\n"
	    "|     |     |     |     |     |     |     |     |     |     |\n"
	    "|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |\n"
	    "|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|"
    };
};

// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Synth demo;

	// Create "screen" of 640x360 "pixels"
	// with a pixel size of 2x2 actual screen pixels
	PGEConfig config;
	config.bVSync = false;
	config.vPixelSize = { 2,2 };
	config.vScreenSize = { 640,360 };

	if (demo.Construct(config))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}