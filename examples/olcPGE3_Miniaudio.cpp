/*
	olc::PixelGameEngine3 Example - olcPGEX3_miniaudio

	Demonstrates using olcPGEX3_miniaudio

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

#define OLC_PGEX3_MINIAUDIO
#include "extensions/olcPGEX3_miniaudio.h"

// Example application demonstrating the miniaudio extension.
// This class overrides the olc::PixelGameEngine base class
// by implementing the OnUserCreate() and OnUserUpdate()
// functions
class Example_Miniaudio : public olc::PixelGameEngine
{
public:
	Example_Miniaudio()
	{
		sAppName = "Example - olcPGEX3_miniaudio";
    	if(!InstallSystemExtension(&audio))
			throw std::runtime_error("Failed to install olcPGEX3_miniaudio");
	}

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// load `assets/song1.mp3` into `song1`
		audio.CreateSoundFromFile(song1, "assets/song1.mp3");
		
        /**
         * this is here to demonstrate how the adventurous can
         * exploit other features of miniaudio that hasn't been
         * abstracted by the PGEX
         * 
         * Here you get a pointer to a next active voice, or the
		 * currently playing voice.
         */
        ma_sound_set_position(song1.GetMASound(), 0.0f, 0.0f, 0.0f);

		// load `assets/SampleA.wav` into `sample`
		audio.CreateSoundFromFile(sample, "assets/SampleA.wav");
        
		// create all of the waveforms at 0.1 amplitude at 440Mhz (A4)
		audio.CreateWaveform(sine,     olc::Miniaudio::Waveform::Type::Sine, 0.1, 440.0);
		audio.CreateWaveform(square,   olc::Miniaudio::Waveform::Type::Square, 0.1, 440.0);
		audio.CreateWaveform(triangle, olc::Miniaudio::Waveform::Type::Triangle, 0.1, 440.0);
		audio.CreateWaveform(sawtooth, olc::Miniaudio::Waveform::Type::Sawtooth, 0.1, 440.0);
		
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// toggle background playback
		if(keyboard.GetKey(olc::Key::K1).bPressed)
		{
			backgroundPlay = !backgroundPlay;
			if(backgroundPlay)
				audio.EnableBackgroundPlayback();
			else
				audio.DisableBackgroundPlayback();
		}

		// ensure all waveforms are stopped
		// before we check for held keys
		sine.Stop(); square.Stop();
		triangle.Stop(); sawtooth.Stop();

		// play `sine` when held
		if(keyboard.GetKey(olc::Key::K7).bHeld)
			sine.Play();

		// play `sqaure` when held
		if(keyboard.GetKey(olc::Key::K8).bHeld)
			square.Play();

		// play `triangle` when held
		if(keyboard.GetKey(olc::Key::K9).bHeld)
			triangle.Play();

		// play `sawtooth` when held
		if(keyboard.GetKey(olc::Key::K0).bHeld)
			sawtooth.Play();

		// toggle `song1` playback/pause
		if(keyboard.GetKey(olc::Key::SPACE).bPressed)
			song1.Toggle();

		// play `sample`
		if(keyboard.GetKey(olc::Key::S).bPressed)
			sample.Play();
		
		// panning
        if(keyboard.GetKey(olc::Key::MINUS).bHeld)
            pan -= 1.0f * fElapsedTime;
        
        if(keyboard.GetKey(olc::Key::EQUALS).bHeld)
            pan += 1.0f * fElapsedTime;
		
		// pitch
        if(keyboard.GetKey(olc::Key::OEM_4).bHeld)
            pitch -= 1.0f * fElapsedTime;

        if(keyboard.GetKey(olc::Key::OEM_6).bHeld)
            pitch += 1.0f * fElapsedTime;
		
		// volume
        if(keyboard.GetKey(olc::Key::DOWN).bHeld)
            volume -= 1.0f * fElapsedTime;
            
        if(keyboard.GetKey(olc::Key::UP).bHeld)
            volume += 1.0f * fElapsedTime;
        
		// distance
        if(keyboard.GetKey(olc::Key::LEFT).bHeld)
            distance -= 10.0f * fElapsedTime;
            
        if(keyboard.GetKey(olc::Key::RIGHT).bHeld)
            distance += 10.0f * fElapsedTime;

        // Reset pan, pitch, and volume
        if(keyboard.GetKey(olc::Key::R).bPressed)
        {
            pan = 0.0f;
            pitch = 1.0f;
            volume = 1.0f;
            distance = 0.0f;
        }
		
		// panning
		pan = std::clamp(pan, -1.0f, 1.0f);
		song1.SetPan(pan);
		
		// pitch
		pitch = std::clamp(pitch, 0.0f, 2.0f);
		song1.SetPitch(pitch);
		
		// volume
		volume = std::clamp(volume, 0.0f, 1.0f);
		song1.SetVolume(volume);

		// this is here to demosntrate how the adventurous can exploit other
		// features of miniaudio that haven't been abstracted by the PGEX.
        distance = std::clamp(distance, 0.0f, 100.0f);
        ma_engine_listener_set_position(&audio.GetEngine(), 0, 0.0f, distance, 0.0f);

		// // get float cursor. 0.0f to 1.0f
 		cursorFloat  = song1.GetCursorFloat();
		// // get cursor in milliseconds
		cursorMillis = song1.GetCursor();

		// Clear whole screen
		draw.Clear(olc::Colour::BLACK);

		if(song1.IsPlaying())
			draw.Clear(olc::Colour::VERY_DARK_BLUE);
		
		draw.String(
			{10, 10},
            "--CONTROLS---------INFORMATION--------------\n"
			"\n"
            "    - =     | Pan      <" + std::to_string(pan) + ">\n"
			"\n"
            "    [ ]     | Pitch    <" + std::to_string(pitch)  + ">\n"
			"\n"
            "   Up Down  | Volume   <" + std::to_string(volume) + ">\n"
			"\n"
            " Left Right | Distance <" + std::to_string(distance) + ">\n"
			"\n"
            "     S      | One-Off Sounds\n"
			"\n"
            "    K1      | BackgroundPlay <" + ((backgroundPlay) ? "On": "Off") + ">\n",
        	olc::Colour::WHITE
		);
		
		draw.String(
			{480, 10},
			"Controls - Waveform""\n"
			"-------------------"
			"\n\n"
			"      K7 | Sine"
			"\n\n"
			"      K8 | Square"
			"\n\n"
			"      K9 | Triangle"
			"\n\n"
			"      K0 | Sawtooth",
			olc::Colour::WHITE
		);
		
		olc::vi2d center = ScreenSize() / 2;
		olc::vf2d scale{2.8f, 2.8f};
		std::string demoMessage = "olcPGEX3_miniaudio Demo";

		draw.String(
			center - olc::vi2d{0, 32} - (draw.GetTextSize(demoMessage, false, scale) / 2),
			demoMessage,
			olc::Colour::WHITE,
			scale
		);
		
		scale = {1.5f, 1.5f};
 		demoMessage = "Hit <SPACE> To Toggle Playback";
		draw.String(
			center - (draw.GetTextSize(demoMessage, false, scale) / 2),
			demoMessage,
			olc::Colour::WHITE,
			scale
		);
		
		demoMessage = "Hit <R> TO Reset Pan/Pitch/Volume";
		draw.String(
			center + olc::vi2d{0, 24} - (draw.GetTextSize(demoMessage, false, scale) / 2),
			demoMessage,
			olc::Colour::WHITE,
			scale
		);

		draw.String(
			{10, 288},
			"Cursor (ms):    " + std::to_string(cursorMillis) + "\n"
			"Cursor (float): " + std::to_string(cursorFloat),
			olc::Colour::WHITE
		);
        
		draw.String(
			{10, 320},
            "Music: Joy Ride [Full version] by MusicLFiles\n"
            "Free download: https://filmmusic.io/song/11627-joy-ride-full-version\n"
            "Licensed under CC BY 4.0: https://filmmusic.io/standard-license\n",
	        olc::Colour::WHITE
		);

        // Draw The Playback Cursor (aka the position in the sound file)
        draw.FilledRect({0, 350}, {ScreenSize().x * cursorFloat, 20}, olc::Colour::YELLOW);

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
		return true;
#else		
		return !keyboard.GetKey(olc::Key::ESCAPE).bPressed;
#endif
	}

	// put this here to have access to audio!
	olc::Miniaudio::AudioEngine audio;

private:	
	// sounds
	olc::Miniaudio::Sound song1;
	olc::Miniaudio::Sound sample;

	olc::Miniaudio::Waveform sine;
	olc::Miniaudio::Waveform square;
	olc::Miniaudio::Waveform triangle;
	olc::Miniaudio::Waveform sawtooth;

    // For demonstration controls, with sensible default values
    float pan    = 0.0f;
    float pitch  = 1.0f;
    float volume = 1.0f;
    float distance = 0.0f;
    bool backgroundPlay = false;
    ma_uint64 cursorMillis = 0ull;
    float     cursorFloat  = 0.0f;
	
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Miniaudio demo;

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
