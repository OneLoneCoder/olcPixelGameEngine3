/*
	olc::PixelGameEngine3 Example - Touch

	Demonstrates touch input handling

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating touch input handling. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Touch : public olc::PixelGameEngine
{
public:
	Example_Touch()
	{
		sAppName = "Example - Touch";
	}

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Nothing to do here, so return true
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// All "touches" get given a unique ID and are tracked by the OS. This ID
		// could be anything, so we never really want to work with t directly.
		auto touches = GetTouch().GetTouchIDs();

		// Display total number of touches
		draw.String({ 2.0f, 2.0f }, "Touch Points: " + std::to_string(touches.size()), olc::Colour::WHITE);


		// Draw Touches
		for (auto id : touches)
		{
			// Get an olc::TouchPoint for this touch
			auto& t = touch.GetTouch(id);

			if (t.bStylus)
			{
				// Touch comes from a stylus, so draw a small circle at the tip position, with size based on the touch size
				draw.Circle(t.position, t.pressure * 10.0f, olc::Colour::WHITE);

				// Stylus *MAY* also have an orientation, so draw a line from the tip in the direction of the stylus
				draw.Line(t.position, t.position + olc::vf2d{ std::cos(t.orientation), std::sin(t.orientation) } * 10.0f * t.pressure, olc::Colour::WHITE);
			}
			else
			{
				// Touch comes from a finger, so draw a circle at the touch position, with size based on the touch size
				draw.FilledRect(t.position, t.size, olc::Colour::YELLOW);
				// Draw a smaller circle at the touch position, with size based on the touch pressure
				draw.FilledRect(t.position, t.size, olc::Colour::YELLOW);
			}
		}
		
		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Touch demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}