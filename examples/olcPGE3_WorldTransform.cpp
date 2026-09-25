/*
	olc::PixelGameEngine3 Example - World Transformations

	Demonstrates how PGE Drawing functions obey a global
	world transform.
	
	In this example, the world transform is manipulated 
	using the arrow keys and Q/A for rotation. 
	
	Holding SHIFT allows you to scale the world instead 
	of translating it.

	SPACE resets the world transform to the default state.

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating world transforms. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_WorldTransform: public olc::PixelGameEngine
{
public:
	Example_WorldTransform()
	{
		sAppName = "Example - World Transformations";
	}

protected:

	// An Image object
	olc::Image imgTest;

	// Handy graph paper for pixel testing
	olc::Image imgGraph;

	olc::vf2d vWorldOffset = { 0.0f, 0.0f };
	olc::vf2d vWorldScale = { 1.0f, 1.0f };
	float fWorldRotation = 0.0f;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Load asset
		CreateImageFromFile(imgTest, "./assets/minsanity_texture.png");

		// Load Graph Paper
		CreateImageFromFile(imgGraph, "./assets/graph_paper.png");
		return true;
	}

	void DrawScene(olc::Pixel col)
	{
		// Draw graph paper background
		draw.Image(imgGraph, { -512,-512 }, { 1, 1 }, col);
		// Draw Triangle and Circle
		draw.Triangle({ 64.0f, 64.0f }, { 128.0f, 96.0f }, { 32.0f, 192.0f }, col);
		draw.Circle({ 256.0f, 256.0f }, 64.0f, col);
		// Draw Sprite
		draw.Image(imgTest, { 192.0f, 64.0f }, { 1.0f, 1.0f }, col);
	}



	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Get Mouse in screen space
		olc::vf2d vMouseScreen = mouse.GetPosition();

		
		// Keyboard handling for world transform
		if (keyboard.GetKey(olc::Key::SHIFT).bHeld)
		{
			if (keyboard.GetKey(olc::Key::LEFT).bPressed)
				vWorldScale.x -= 0.125;
			if (keyboard.GetKey(olc::Key::RIGHT).bPressed)
				vWorldScale.x += 0.125;
			if (keyboard.GetKey(olc::Key::UP).bPressed)
				vWorldScale.y -= 0.125;
			if (keyboard.GetKey(olc::Key::DOWN).bPressed)
				vWorldScale.y += 0.125;
		}
		else
		{
			if (keyboard.GetKey(olc::Key::LEFT).bHeld)
				vWorldOffset.x -= 64.0f * fElapsedTime;
			if (keyboard.GetKey(olc::Key::RIGHT).bHeld)
				vWorldOffset.x += 64.0f * fElapsedTime;
			if (keyboard.GetKey(olc::Key::UP).bHeld)
				vWorldOffset.y -= 64.0f * fElapsedTime;
			if (keyboard.GetKey(olc::Key::DOWN).bHeld)
				vWorldOffset.y += 64.0f * fElapsedTime;
		}

		if (keyboard.GetKey(olc::Key::Q).bHeld)
			fWorldRotation -= 0.5f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::A).bHeld)
			fWorldRotation += 0.5f * fElapsedTime;

		if (keyboard.GetKey(olc::Key::SPACE).bPressed)
		{
			vWorldOffset = { 0.0f, 0.0f };
			vWorldScale = { 1.0f, 1.0f };
			fWorldRotation = 0.0f;
		}

		// Clamp world transform values to reasonable limits
		vWorldOffset = vWorldOffset.clamp({ -512.0f, -512.0f }, { 512.0f, 512.0f });
		vWorldScale = vWorldScale.clamp({ 0.125f, 0.125f }, { 4.0f, 4.0f });

		// Draw in white the default transform (world offset 0,0, scale 1,1, rotation 0)		
		draw.WorldReset();
		DrawScene(olc::Colour::WHITE);

		// Draw in yellow the transformed world
		draw.WorldScale(vWorldScale);
		draw.WorldRotate(fWorldRotation);
		draw.WorldOffset(vWorldOffset);
		DrawScene(olc::Colour::YELLOW);

		// Get Mouse in "transformed world" space
		olc::vf2d vMouseWorld = draw.ScreenToWorld(vMouseScreen);

		// Unpack the mouse coordinate to x and y just for fun and to serve as a compilation test for unpacking v_2d objects
		const auto& [x1, y1] = vMouseWorld;
		auto& [x2, y2] = vMouseWorld;
		auto [x3, y3] = vMouseWorld;
		auto&& [x4, y4] = vMouseWorld;

		// Draw Status
		draw.WorldReset();
		draw.StringProp({ 8, 300 }, "Offset: Arrow Keys, Scale: Shift & Arrow Keys, Rotate: Q/A, SPACE: Reset");		
		draw.StringProp({ 8, 330 }, "World Offset: " + vWorldOffset.str());
		draw.StringProp({ 8, 340 }, "World Scale: " + vWorldScale.str());
		draw.StringProp({ 8, 350 }, "World Rotation: " + std::to_string(fWorldRotation));
		draw.StringProp({ 8, 360 }, "Mouse in screen space: " + vMouseScreen.str());
		draw.StringProp({ 8, 370 }, "Mouse in world space: " + vMouseWorld.str());

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_WorldTransform demo;

	// Create "screen" of 512x480 "pixels"
	// with a pixel size of 2x2 actual screen pixels
	if (demo.Construct({ 512, 480 }, { 2, 2 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}