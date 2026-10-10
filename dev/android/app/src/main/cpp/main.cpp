/*
	olc::PixelGameEngine3 Example - Lines

	Draws a few lines using different styles and techniques

	Licenced under the OLC-3 License
*/

// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

// Example application demonstrating various line drawing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class olcPixelGameEngine3 : public olc::PixelGameEngine
{
public:
	olcPixelGameEngine3()
	{
		sAppName = "olcPixelGameEngine3";
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

		// Simple white line
		draw.Line({ 10.0f, 10.0f }, { 246.0f, 40.0f }, olc::Colour::WHITE);

		// Gradient line
		draw.Line({ 10.0f, 50.0f }, olc::Colour::RED, { 246.0f, 80.0f }, olc::Colour::YELLOW);
		
		// Rotating gradient lines
		float fTotalTime = TotalTimeElapsed();
		olc::vf2d p1 = { 64.0f, 160.0f };
		olc::vf2d p2 = olc::vf2d{ std::cos(fTotalTime), std::sin(fTotalTime) } * 64.0f;

		draw.Line(p1, olc::Colour::CYAN, p1 + p2, olc::Colour::MAGENTA);
		draw.Line(p1, olc::Colour::CYAN, p1 - p2, olc::Colour::MAGENTA);
		draw.Line(p1, olc::Colour::CYAN, p1 + olc::vf2d{ -p2.y, p2.x }, olc::Colour::MAGENTA);
		draw.Line(p1, olc::Colour::CYAN, p1 + olc::vf2d{ p2.y, -p2.x }, olc::Colour::MAGENTA);

		// Line to mouse pointer with tint
		float alpha = (std::sin(fTotalTime * 4.0f) + 1.0f) / 2.0f;
		draw.Line(p1, mouse.GetPosition(), olc::Colour::WHITE, olc::PixelF(1.0f, 0.0f, 0.0f, alpha));

		// Boundary box (with fixed	coordinates)
		draw.Line({ 0.0f, 0.0f }, { 255.0f, 0.0f }, olc::Colour::DARK_GREY);
		draw.Line({ 255.0f, 0.0f }, { 255.0f, 239.0f }, olc::Colour::DARK_GREY);
		draw.Line({ 255.0f, 239.0f }, { 0.0f, 239.0f }, olc::Colour::DARK_GREY);
		draw.Line({ 0.0f, 239.0f }, { 0.0f, 0.0f }, olc::Colour::DARK_GREY);

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main(int argc, char** argv)
{
	// Construct application
	olcPixelGameEngine3 game;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (game.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		game.Start();
	}

	return 0;
}