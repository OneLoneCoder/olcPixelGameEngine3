/*
	olc::PixelGameEngine3 Example - Keybaord (sic :P)

	Demonstrates using the keyboard input system

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating keyboard input. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Keyboard : public olc::PixelGameEngine
{
public:
	Example_Keyboard()
	{

	}

protected:
	
	olc::vf2d vPosition;
	std::string sText;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		vPosition = ScreenSize() / 2;
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Move position with arrow keys
		if (keyboard.GetKey(olc::Key::UP).bHeld)	
			vPosition.y -= 50.0f * fElapsedTime;

		if (keyboard.GetKey(olc::Key::DOWN).bHeld)
			vPosition.y += 50.0f * fElapsedTime;

		if (keyboard.GetKey(olc::Key::LEFT).bHeld)
			vPosition.x -= 50.0f * fElapsedTime;

		if (keyboard.GetKey(olc::Key::RIGHT).bHeld)
			vPosition.x += 50.0f * fElapsedTime;

		draw.FilledCircle(vPosition.round(), 10.0f);


		// Capture text input from keyboard
		// Note: this is specifically for text entry and shouldnt
		// be used for key polling.
		for (const auto& key : keyboard.GetKeyCache())
		{
			// Each olc::Key has a glyph associated with it
			// depending on the current keyboard layout, which
			// can be queried via GetKeyGlyph()
			if (key == olc::Key::BACK && !sText.empty())
				sText.pop_back();
			else
				sText += keyboard.GetKeyGlyph(key, keyboard.GetKey(olc::Key::SHIFT).bHeld);
		}

		// Display current keyboard layout
		std::string sKeyboardType;
		auto kbl = pHost->GetKeyboardLayout();
		if(kbl == olc::KeyboardLayout::QWERTY_UK)
			sKeyboardType = "QWERTY UK";
		else if (kbl == olc::KeyboardLayout::QWERTY_US)
			sKeyboardType = "QWERTY US";
		else if (kbl == olc::KeyboardLayout::AZERTY)
			sKeyboardType = "AZERTY";
		else if (kbl == olc::KeyboardLayout::QWERTZ)
			sKeyboardType = "QWERTZ";
		else
			sKeyboardType = "UNKNOWN";

		draw.StringProp({ 10, 5 }, "Keyboard Layout: " + sKeyboardType, olc::Colour::YELLOW);
		draw.StringProp({ 10,20 }, sText, olc::Colour::GREEN);
		

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Keyboard demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}