/*
	olc::PixelGameEngine3 Example - olcPGEX3_Miniaudio

	Demonstrates using olcPGEX3_Miniaudio

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

#define OLC_PGEX3_MINIAUDIO
#include "olcPGEX3_Miniaudio.h"

// Save us a whole lot of typing in this demo
using namespace olc::ext::Miniaudio;

// Example application demonstrating the miniaudio extension.
// This class overrides the olc::PixelGameEngine base class
// by implementing the OnUserCreate() and OnUserUpdate()
// functions
class Example_Waveforms : public olc::PixelGameEngine
{
public:
	Example_Waveforms()
	{
		sAppName = "Example - olcPGEX3_Miniaudio";
    	if(!InstallSystemExtension(&audio))
			throw std::runtime_error("Failed to install olcPGEX3_Miniaudio");
	}

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		auto CreateWaveform = [&](olc::Key key, Waveform::Type type, double frequency)
		{
			Waveform w;
			audio.CreateWaveform(w, type, 0.01, frequency);
			mapKeys[key] = w;
		};
		
		CreateWaveform(olc::Key::A, Waveform::Type::Sine, 207.65f);
		CreateWaveform(olc::Key::Z, Waveform::Type::Sine, 220.00f);
		CreateWaveform(olc::Key::S, Waveform::Type::Sine, 233.08f);
		CreateWaveform(olc::Key::X, Waveform::Type::Sine, 246.94f);
		CreateWaveform(olc::Key::C, Waveform::Type::Sine, 261.63f);
		CreateWaveform(olc::Key::F, Waveform::Type::Sine, 277.18f);
		CreateWaveform(olc::Key::V, Waveform::Type::Sine, 293.66f);
		CreateWaveform(olc::Key::G, Waveform::Type::Sine, 311.13f);
		CreateWaveform(olc::Key::B, Waveform::Type::Sine, 329.63f);
		CreateWaveform(olc::Key::N, Waveform::Type::Sine, 349.23f);
		CreateWaveform(olc::Key::J, Waveform::Type::Sine, 369.99f);
		CreateWaveform(olc::Key::M, Waveform::Type::Sine, 392.00f);
		CreateWaveform(olc::Key::K, Waveform::Type::Sine, 415.30f);
		CreateWaveform(olc::Key::COMMA, Waveform::Type::Sine, 440.00f);
		CreateWaveform(olc::Key::L, Waveform::Type::Sine, 466.16f);
		CreateWaveform(olc::Key::PERIOD, Waveform::Type::Sine, 493.88f);
		CreateWaveform(olc::Key::OEM_2, Waveform::Type::Sine, 523.25f);
		
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		if(keyboard.GetKey(olc::Key::LEFT).bPressed)
		{
			waveformTypeTracker = ((waveformTypeTracker + (int)Waveform::Type::Count) - 1) % (int)Waveform::Type::Count;
			for(auto& i : mapKeys)
				audio.SetWaveformType(i.second, (Waveform::Type)waveformTypeTracker);
		}
		
		if(keyboard.GetKey(olc::Key::RIGHT).bPressed)
		{
			waveformTypeTracker = (waveformTypeTracker + 1) % (int)Waveform::Type::Count;
			for(auto& i : mapKeys)
				audio.SetWaveformType(i.second, (Waveform::Type)waveformTypeTracker);
		}

		for(auto& i : mapKeys)
		{
			audio.Stop(i.second);
			if(keyboard.GetKey(i.first).bHeld)
				audio.Play(i.second);
		}

		draw.Clear(olc::Colour::VERY_DARK_BLUE);

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
		demoMessage = "Hit <Left> or <Right> To Change Waveform Type";
		draw.String(
			center - (draw.GetTextSize(demoMessage, false, scale) / 2),
			demoMessage,
			olc::Colour::WHITE,
			scale
		);

		demoMessage = "Current Wave (" + waveformTypeToName.at((Waveform::Type)waveformTypeTracker) + ")";
		draw.String(
			center + olc::vi2d{0, 24} - (draw.GetTextSize(demoMessage, false, scale) / 2),
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

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
		return true;
#else		
		return !keyboard.GetKey(olc::Key::ESCAPE).bPressed;
#endif
	}

	// put this here to have access to audio!
	olc::ext::Miniaudio::AudioEngine audio;

private:	
	int waveformTypeTracker = 0;
    const std::unordered_map<Waveform::Type,std::string> waveformTypeToName
    {
        {Waveform::Type::Sine, "SINE"},
        {Waveform::Type::Square, "SQUARE"},
        {Waveform::Type::Triangle, "TRIANGLE"},
        {Waveform::Type::Sawtooth, "SAWTOOTH"},
    };

	std::unordered_map<olc::Key, Waveform> mapKeys;
    
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
	Example_Waveforms demo;

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
