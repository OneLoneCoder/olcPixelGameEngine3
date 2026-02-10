#pragma once

//! START STDHEADER GLOBAL
#include <cstdint>
#include <string>
#include <array>
#include <vector>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "pixel.h"
#include "image.h"
#include "transform2d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_GPUTASK_DECLARED)
namespace olc
{
		
	// Define super structure to be drawn
	enum class Structure : uint8_t
	{
		// Vertex buffer is a series of points	
		Point = 0,
		// Vertex buffer is a series of connected line segments
		Line,
		// Vertex buffer is a series of connected line segments, that close to form a loop
		LineLoop,
		// Vertex buffer is a series of individual line segments
		LineList,
		// Vertex buffer is a fan of triangles
		Fan,
		// Vertex buffer is a strip of adjacent triangles
		Strip,
		// Vertex buffer is a series of discrete triangles
		List, 
	};
	
	// This is the default "packet" of work that is sent to 
	// a GPU for drawing. Various drawing operations throughout
	// PGE create GPUTasks which are stored and dispatched when
	// appropriate. It's contents are purposefully abstract
	// to ensure rendering tools do not need to be reliant
	// upon PGE structures
	struct GPUTask
	{
		enum class Task : uint8_t
		{
			DrawPolygon,
			NullTask
		} task = Task::DrawPolygon;

		struct Vertex 
		{
			float p[4] = { 0,0,1,1 };     // x, y, z, w
			olc::Pixel c = olc::Colour::WHITE;	// 32-bit colour
			float t0[2]{};
			float t1[2]{};
			float t2[2]{};
			float t3[2]{};
		};

		// Simple vertex buffer
		std::vector<Vertex> vertexBuffer;

		// Model-View-Projection Matrix for shader
		std::array<float, 16> mvpMatrix = { {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		} };

		// Use depth components
		bool bDepth = false;

		// Use hardware wire drawing
		bool bWireframe = false;

		// Define how to interpret vertex buffer
		bool bIs3D = false;

		// Overall biasing colour (great for blends)
		olc::Pixel tint = olc::Colour::WHITE;

		olc::Image* pImage = nullptr;

		// Define super structure to be drawn
		Structure structure = Structure::Fan;

		// Define if GPU should face cull based on winding order
		enum class CullMode : uint8_t
		{
			// No
			None = 0,
			// Cull if vertices are listed in clockwise order
			ClockWise,
			// Cull if vertices are listed in anticlockwise order
			CounterClockWise
		} cullmode = CullMode::None;
	};
}
#define PGE_GPUTASK_DECLARED 1
#endif
//! END DECLARATION