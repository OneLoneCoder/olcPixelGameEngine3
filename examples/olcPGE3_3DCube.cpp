/*
	olc::PixelGameEngine3 Example - olc::SanityCube!!!

	Draws teh infamous olc::SanityCube using the hardware 3D rendering capabilities. 
	
	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating basic 3D rendering. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_3DCube : public olc::PixelGameEngine
{
public:
	Example_3DCube()
	{

	}

protected:

	// Matrices for 3D projection, view and world transforms
	olc::mf4d matProj;
	olc::mf4d matView;
	olc::mf4d matWorld;

	// Position in 3D space of "the camera"
	olc::vf4d vViewTranslate = { 0.0f, 0.0f, 2.5f };

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Load the image for the sanity cube, and create the mesh for it
		CreateImageFromFile(imSanityCube, "./assets/sanity_cube.png");
		meshSanityCube = CreateSanityCube();

		// Only needs setting once, as the projection matrix doesn't change in this example
		matProj.perspective(90.0f * 3.14159f / 180.0f, float(ScreenSize().x) / float(ScreenSize().y), 0.1f, 100.0f);
		draw.SetProjectionMatrix(matProj);

		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Handle some input to coarsely move the 3D view around. Note
		// we have no notion of a camera without any additonal utilities
		if (keyboard.GetKey(olc::Key::LEFT).bHeld)
			vViewTranslate.x -= 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::RIGHT).bHeld)
			vViewTranslate.x += 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::UP).bHeld)
			vViewTranslate.y += 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::DOWN).bHeld)
			vViewTranslate.y -= 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::Q).bHeld)
			vViewTranslate.z += 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::A).bHeld)
			vViewTranslate.z -= 5.0f * fElapsedTime;

		if (keyboard.GetKey(olc::Key::SPACE).bPressed)
			bSpinning = !bSpinning;


		// Clear whole screen (and depth buffer - very important!)
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Draw Background Gradient using draw2d, to show that 
		// draw3d doesn't mess with draw2d's state
		draw.FilledRect({ 0, 0 }, draw.GetTargetSize(),
			olc::Colour::WHITE, olc::Colour::YELLOW,
			olc::Colour::CYAN, olc::Colour::MAGENTA);
	
		// Construct the view matrix. Now... apologies...
		// PGE3 has a left-handed coordinate system, with the camera looking down the 
		// positive Z axis. This is a bit unusual, but it means that the default view 
		// matrix needs to be rotated 180 degrees around the X axis.
		olc::mf4d matViewRotateX, matViewTranslate;
		matViewRotateX.rotateX(3.14159f);
		matViewTranslate.translate(vViewTranslate);
		matView = matViewRotateX * matViewTranslate;
		draw.SetViewMatrix(matView);
			
		// Create a world matrix that rotates the cube over time. The cube is offset
		// so it rotates around its centre. Its verts are defined in the range 0..1
		olc::mf4d matTrans, matRotX, matRotY;
		matTrans.translate(-0.5f, -0.5f, -0.5f);
		if (bSpinning)
		{
			matRotX.rotateX(TotalTimeElapsed() * 0.5f);
			matRotY.rotateY(TotalTimeElapsed() * 0.25f);
		}
		
		// Combine transformations & Apply:
		// 1. Translate the cube so its centre is at the origin
		// 2. Rotate the cube around the Y axis
		// 3. Rotate the cube around the X axis
		matWorld = matRotX * matRotY * matTrans;
		draw.SetModelMatrix(matWorld);

		// The olc::SanityCube (TM) (c) is defined with vertices in clockwise order, 
		// so cull counter-clockwise faces to show it off in all its glory! This is
		// counter to OpenGL's default culling mode, so it's a good test of the culling 
		// system as well.
		draw.SetCullMode(olc::GPUTask::CullMode::CounterClockWise);
		
		// Draw the cube using the sanity cube's layout, and vectors of vertices, colours
		// and texture coordinates.		 
		draw.Mesh(meshSanityCube.layout, meshSanityCube.pos, meshSanityCube.col, meshSanityCube.uv, imSanityCube);

		// Draw a little RGB axis indicator
		matWorld.translate(-1,-1,-1);
		draw.SetModelMatrix(matWorld);
		draw.Line({ 0,0,0 }, { 1, 0, 0 }, olc::Colour::RED);
		draw.Line({ 0,0,0 }, { 0, 1, 0 }, olc::Colour::GREEN);
		draw.Line({ 0,0,0 }, { 0, 0, 1 }, olc::Colour::BLUE);

		draw.StringProp({ 4, 4 }, "+X: Right\n-X: Left\n+Y: Up\n-Y: Down\n+Z: Q\n-Z: A\nSPIN: Space", olc::Colour::BLACK);

		// Successful frame
		return true;
	}

	// A simple mesh structure, containing vectors of vertex attributes
	struct mesh
	{
		std::vector<olc::vf4d> pos;
		std::vector<olc::vf4d> norm;
		std::vector<olc::vf2d> uv;
		std::vector<olc::Pixel> col;
		olc::Structure layout = olc::Structure::List;
	};

	// The image and mesh for the sanity cube
	olc::Image imSanityCube;
	mesh meshSanityCube;

	bool bSpinning = true;


	// Behold!! The Sanity Cube!! A cube with all the correct vertex attributes, to 
	// be used as a sanity check for the 3D rendering pipeline. 
	// If this doesn't render correctly, then nothing will.
	inline mesh CreateSanityCube()
	{
		mesh m;
		m.layout = olc::Structure::List;

		// South
		m.pos.push_back({ 0,0,0 }); m.norm.push_back({ 0, 0, -1, 0 }); m.uv.push_back({ 0.25, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,1,0 }); m.norm.push_back({ 0, 0, -1, 0 }); m.uv.push_back({ 0.25, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,1,0 }); m.norm.push_back({ 0, 0, -1, 0 }); m.uv.push_back({ 0.5, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,0,0 }); m.norm.push_back({ 0, 0, -1, 0 }); m.uv.push_back({ 0.25, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,1,0 }); m.norm.push_back({ 0, 0, -1, 0 }); m.uv.push_back({ 0.5, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,0,0 }); m.norm.push_back({ 0, 0, -1, 0 }); m.uv.push_back({ 0.5, 0.5 }); m.col.push_back(olc::Colour::WHITE);

		// East
		m.pos.push_back({ 1,0,0 }); m.norm.push_back({ 1, 0, 0, 0 }); m.uv.push_back({ 0.5, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,1,0 }); m.norm.push_back({ 1, 0, 0, 0 }); m.uv.push_back({ 0.5, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,1,1 }); m.norm.push_back({ 1, 0, 0, 0 }); m.uv.push_back({ 0.75, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,0,0 }); m.norm.push_back({ 1, 0, 0, 0 }); m.uv.push_back({ 0.5, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,1,1 }); m.norm.push_back({ 1, 0, 0, 0 }); m.uv.push_back({ 0.75, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,0,1 }); m.norm.push_back({ 1, 0, 0, 0 }); m.uv.push_back({ 0.75, 0.5 }); m.col.push_back(olc::Colour::WHITE);

		// North
		m.pos.push_back({ 1,0,1 }); m.norm.push_back({ 0, 0, 1, 0 }); m.uv.push_back({ 0.75, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,1,1 }); m.norm.push_back({ 0, 0, 1, 0 }); m.uv.push_back({ 0.75, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,1,1 }); m.norm.push_back({ 0, 0, 1, 0 }); m.uv.push_back({ 1.0, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,0,1 }); m.norm.push_back({ 0, 0, 1, 0 }); m.uv.push_back({ 0.75, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,1,1 }); m.norm.push_back({ 0, 0, 1, 0 }); m.uv.push_back({ 1.0, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,0,1 }); m.norm.push_back({ 0, 0, 1, 0 }); m.uv.push_back({ 1.0, 0.5 }); m.col.push_back(olc::Colour::WHITE);

		// West
		m.pos.push_back({ 0,0,1 }); m.norm.push_back({ -1, 0, 0, 0 }); m.uv.push_back({ 0.0, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,1,1 }); m.norm.push_back({ -1, 0, 0, 0 }); m.uv.push_back({ 0.0, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,1,0 }); m.norm.push_back({ -1, 0, 0, 0 }); m.uv.push_back({ 0.25, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,0,1 }); m.norm.push_back({ -1, 0, 0, 0 }); m.uv.push_back({ 0.0, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,1,0 }); m.norm.push_back({ -1, 0, 0, 0 }); m.uv.push_back({ 0.25, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,0,0 }); m.norm.push_back({ -1, 0, 0, 0 }); m.uv.push_back({ 0.25, 0.5 }); m.col.push_back(olc::Colour::WHITE);

		// Top
		m.pos.push_back({ 0,1,0 }); m.norm.push_back({ 0, 1, 0, 0 }); m.uv.push_back({ 0.25, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,1,1 }); m.norm.push_back({ 0, 1, 0, 0 }); m.uv.push_back({ 0.25, 0.0 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,1,1 }); m.norm.push_back({ 0, 1, 0, 0 }); m.uv.push_back({ 0.5, 0.0 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,1,0 }); m.norm.push_back({ 0, 1, 0, 0 }); m.uv.push_back({ 0.25, 0.25 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,1,1 }); m.norm.push_back({ 0, 1, 0, 0 }); m.uv.push_back({ 0.5, 0.0 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,1,0 }); m.norm.push_back({ 0, 1, 0, 0 }); m.uv.push_back({ 0.5, 0.25 }); m.col.push_back(olc::Colour::WHITE);

		// Bottom
		m.pos.push_back({ 0,0,0 }); m.norm.push_back({ 0, -1, 0, 0 }); m.uv.push_back({ 0.25, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,0,0 }); m.norm.push_back({ 0, -1, 0, 0 }); m.uv.push_back({ 0.5, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,0,1 }); m.norm.push_back({ 0, -1, 0, 0 }); m.uv.push_back({ 0.5, 0.75 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,0,0 }); m.norm.push_back({ 0, -1, 0, 0 }); m.uv.push_back({ 0.25, 0.5 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 1,0,1 }); m.norm.push_back({ 0, -1, 0, 0 }); m.uv.push_back({ 0.5, 0.75 }); m.col.push_back(olc::Colour::WHITE);
		m.pos.push_back({ 0,0,1 }); m.norm.push_back({ 0, -1, 0, 0 }); m.uv.push_back({ 0.25, 0.75 }); m.col.push_back(olc::Colour::WHITE);

		return m;
	}



};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_3DCube demo;

	olc::PGEConfig config;
	config.vScreenSize = { 256, 240 };
	config.vPixelSize = { 4, 4 };
	config.bAntiAliasMainScreen = false;

	if (demo.Construct(config))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}