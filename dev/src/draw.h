#pragma once

//! START STDHEADER
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <memory>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "vector2d.h"
#include "transform2d.h"
#include "vector4d.h"
#include "matrix4d.h"
#include "pixel.h"
#include "image.h"
#include "gputask.h"
#include "font.h"
//! END CUSTOMHEADER


/*
	Draw Function Implementation Status Table
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Note 0: [#] = Implemented, [ ] = Not Implemented

	Note 1: Functions that accept a 'batch' parameter add the drawing command to the batch
			and return the batch reference for convenience.

	Note 2: Functions that accept a 'tint' parameter apply the tint to the entire shape.

	Note 3: Functions that accept 'colInner' and 'colOuter' parameters create a radial gradient
			from the center to the edge of the shape.

	Note 4: Functions that accept a 'structure' parameter define how the polygon is constructed
			(e.g., TriangleFan, TriangleStrip, etc.)

	Note 5: Functions that accept 'facets' parameter define the number of segments used to
			approximate curves (circles, ellipses, rounded rectangles).

	Note 6: Functions that accept 'uv' parameters define texture coordinates for textured shapes.
			These are typically in the range [0.0, 1.0].

	Note 7: Functions that accept an 'image' parameter use the specified image for texturing. By
			deafult this is the whole image, but can be a sub-region defined by an ImageRegion.
			ImageRegions are defined in pixel coordinates relative to the top-left of the image.

	Note 8: Batched images retain individual tints, but use image supplied by CreateImageBatch(). The
			image supplied to these functions is actually an image region structure, allowing
			sub-image rendering. When the batch is drawn, a global tint can be supplied.

	Note 9: A batch object must be constructed before use with the appropriate CreateXXXBatch()

	Note A: TexturedXXX batch functions use an image batch, naturally restricting all shapes in the 
			batch to use the same texture source.

			
	[#] Clear(col)		
	[#] Pixel(pos, col)						

	[#] Line(p1, p2, col, [tint])					
	[#] Line(p1, c1, p2, c2, [tint])				
	[#] Line(batch, p1, p2, col)			
	[#] Line(batch, p1, c1, p2, c2)			

	[#] Rect(pos, size, col, [tint])				
	[#] Rect(pos, size, colTL, colTR, colBL, colBR, [tint])	
	[#] Rect(batch, pos, size, col)			
	[#] Rect(batch, pos, size, colTL, colTR, colBL, colBR)	

	[#] FilledRect(pos, size, col, [tint])			
	[#] FilledRect(pos, size, colTL, colTR, colBL, colBR, [tint])	
	[#] FilledRect(batch, pos, size, col)	
	[#] FilledRect(batch, pos, size, colTL, colTR, colBL, colBR)	

	[#] Circle(pos, radius, col, [tint], [facets])			
	[#] Circle(pos, radius, colInner, colOuter, [tint], [facets])	
	[#] Circle(batch, pos, radius, col, [facets])		
	[#] Circle(batch, pos, radius, colInner, colOuter, [facets])	

	[#] FilledCircle(pos, radius, col, [tint], [facets])		
	[#] FilledCircle(pos, radius, colInner, colOuter, [tint], [facets])	
	[#] FilledCircle(batch, pos, radius, col, [facets])	
	[#] FilledCircle(batch, pos, radius, colInner, colOuter, [facets])	

	[#] Ellipse(pos, radiusX, radiusY, col, [tint], [facets])			
	[#] Ellipse(pos, radiusX, radiusY, colInner, colOuter, [tint], [facets])	
	[#] Ellipse(batch, pos, radiusX, radiusY, col, [facets])		
	[#] Ellipse(batch, pos, radiusX, radiusY, colInner, colOuter, [facets])	

	[#] FilledEllipse(pos, radiusX, radiusY, col, [tint], [facets])			
	[#] FilledEllipse(pos, radiusX, radiusY, colInner, colOuter, [tint], [facets])	
	[#] FilledEllipse(batch, pos, radiusX, radiusY, col, [facets])		
	[#] FilledEllipse(batch, pos, radiusX, radiusY, colInner, colOuter, [facets])	

	[#] RoundedRect(pos, size, radius, col, [tint], [facets/4])			
	[#] FilledRoundedRect(pos, size, radius, col, [tint], [facets/4])	

	[ ] RoundedRect(batch, pos, size, radius, col, [facets/4])
	[ ] FilledRoundedRect(batch, pos, size, radius, col, [facets/4])

	[#] Triangle(p1, p2, p3, col, [tint])			
	[#] Triangle(p1, c1, p2, c2, p3, c3, [tint])		
	[#] Triangle(batch, p1, p2, p3, col)	
	[#] Triangle(batch, p1, c1, p2, c2, p3, c3)	

	[#] FilledTriangle(p1, p2, p3, col, [tint])		
	[#] FilledTriangle(p1, c1, p2, c2, p3, c3, [tint])	
	[#] FilledTriangle(batch, p1, p2, p3, col)	
	[#] FilledTriangle(batch, p1, c1, p2, c2, p3, c3)
	
	[#] Polygon(structure, points[], col, [tint])				
	[#] Polygon(structure, points[], colours[], [tint])		
	[ ] Polygon(batch, structure, points[], col)		
	[ ] Polygon(batch, structure, points[], colours[])	

	[#] FilledPolygon(structure, points[], col, [tint])		
	[#] FilledPolygon(structure, points[], colours[], [tint])	
	[ ] FilledPolygon(batch, structure, points[], col)	
	[ ] FilledPolygon(batch, structure, points[], colours[])	

	[#] TexturedTriangle(p1, p2, p3, c1, c2, c3, uv1, uv2, uv3, image, [tint])
	[ ] TexturedTriangle(batch, p1, p2, p3, c1, c2, c3, uv1, uv2, uv3, image)

	[#] TexturedPolygon(structure, points[], colours[], uvs[], image, [tint])	
	[ ] TexturedPolygon(batch, structure, points[], colours[], uvs[], image)

	[#] String(pos, text, col, [scale], [font])
	[#] StringProp(pos, text, col, [scale], [font])
	[ ] String(batch, pos, text, col, [scale], [font])
	[ ] StringProp(batch, pos, text, col, [scale], [font])

	[#] Image(image, pos, [scale], [tint])
	[#] ImageRect(image, pos, size, [tint])
	[#] ImageQuad(image, vTL, vTR, vBL, vBR, [tint])
	[#] ImageQuad(image, points[4], [tint])
	[#] ImageRotated(image, pos, angle, [offset], [scale], [tint])

	[#] Image(batch, image, pos, [scale], [tint])
	[#] ImageRect(batch, image, pos, size, [tint])
	[#] ImageQuad(batch, image, vTL, vTR, vBL, vBR, [tint])
	[#] ImageQuad(batch, image, points[4], [tint])
	[#] ImageRotated(batch, image, pos, angle, [offset], [scale], [tint])

	[#] Batch(LineBtach, [tint])
	[#] Batch(FilledBatch, [tint])
	[#] Batch(ImageBatch, [tint])


	3D Rendering Functions
	~~~~~~~~~~~~~~~~~~~~~~



*/


//! START DECLARATION
#if !defined(PGE_DRAW2D_DECLARED)
namespace olc
{
	namespace gpu
	{
		class Renderer;
		class Shader;
	}

	class Draw3D;
	
	// These "opaque" structs are merely to help with
	// type differentiation of various GPUTask types
	// when batching. For example a batch of lines is
	// different to a batch of filled rectangles, and 
	// needs to be treated differently by the GPU.
	struct ImageBatch { GPUTask task; };
	struct FilledBatch { GPUTask task; };
	struct LineBatch { GPUTask task; };

	class Draw
	{
		friend class olc::Draw3D;

	public:
		Draw();

		// Associate this drawing toolbox with a renderer
		void SetGPU(olc::gpu::Renderer* const renderer);
		void ProcessGPUTasks();

	public:
		// Sets the drawing target of this drawing toolbox
		void SetTarget(olc::Image& image);
		// Get the current drawing target
		olc::Image& GetTarget();
		// Get Size of drawing target (aka GetTarget()->Size())
		olc::vi2d GetTargetSize();

	public: // Affine Transformation (these affect all subsequent draw calls for this target)
		// Reset world transform to identity
		void WorldReset();
		// Apply world scaling
		void WorldScale(const olc::vf2d& vScale);
		// Apply world translation
		void WorldOffset(const olc::vf2d& vOffset);
		// Apply world rotation
		void WorldRotate(const float& fTheta, const olc::vf2d& vPoint = { 0,0 });
		
		// World transforms
		void SetWorldTransform(const olc::tf2d& trans);
		olc::tf2d& GetWorldTransform();

		// Coordinate transforms
		olc::vf2d WorldToScreen(const olc::vf2d& v) const;
		olc::vf2d ScreenToWorld(const olc::vf2d& v) const;

	public: // Primitive Drawing Functions
		// Plot a single pixel
		void Pixel(
			const olc::vf2d& pos, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Read a pixel from an image (guarantees fresh)
		olc::Pixel GetPixel(
			olc::Image& image, 
			const olc::vf2d& pos);

		// Read a pixel from target image (guarantees fresh)
		olc::Pixel GetPixel(			
			const olc::vf2d& pos);

		// Clear entire draw target to specific colour
		void Clear(const olc::Pixel& col);
	
	public: // Shape Drawing Functions


// === Lines ===

		// Draws a single pixel wide line		
		const GPUTask& Line(
			const olc::vf2d& p1, 
			const olc::vf2d& p2, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a single pixel wide line into a batch
		const LineBatch& Line(
			olc::LineBatch& batch,
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::Pixel col = olc::Colour::WHITE);

		// Draws a single pixel wide line with a gradient		
		const GPUTask& Line(
			const olc::vf2d& p1, 
			const olc::Pixel c1, 
			const olc::vf2d& p2, 
			const olc::Pixel c2,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a single pixel wide line with a gradient into a batch	
		const LineBatch& Line(
			olc::LineBatch& batch,
			const olc::vf2d& p1,
			const olc::Pixel c1,
			const olc::vf2d& p2,
			const olc::Pixel c2);

// === Rectangles ===

		// Draws a rectangle outline
		const GPUTask& Rect(
			const olc::vf2d& pos, 
			const olc::vf2d& size, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a rectangle outline into a batch
		const LineBatch& Rect(
			olc::LineBatch& batch,
			const olc::vf2d& pos,
			const olc::vf2d& size,
			const olc::Pixel col = olc::Colour::WHITE);

		// Draws a multiple colour rectangle, with linear colour interpolation
		const GPUTask& Rect(
			const olc::vf2d& pos,
			const olc::vf2d& size,
			const olc::Pixel colTL,
			const olc::Pixel colTR,
			const olc::Pixel colBL,
			const olc::Pixel colBR,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a multiple colour rectangle, with linear colour interpolation, into a batch
		const LineBatch& Rect(
			olc::LineBatch& batch,
			const olc::vf2d& pos,
			const olc::vf2d& size,
			const olc::Pixel colTL,
			const olc::Pixel colTR,
			const olc::Pixel colBL,
			const olc::Pixel colBR);
			
		// Draws a filled, single colour rectangle
		const GPUTask& FilledRect(
			const olc::vf2d& pos, 
			const olc::vf2d& size, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled, single colour rectangle into a batch
		const FilledBatch& FilledRect(
			olc::FilledBatch& batch,
			const olc::vf2d& pos,
			const olc::vf2d& size,
			const olc::Pixel col = olc::Colour::WHITE);

		// Draws a filled, multiple colour rectangle, with linear colour interpolation
		const GPUTask& FilledRect(
			const olc::vf2d& pos, 
			const olc::vf2d& size, 
			const olc::Pixel colTL, 
			const olc::Pixel colTR, 
			const olc::Pixel colBL, 
			const olc::Pixel colBR,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled, multiple colour rectangle, with linear colour interpolation, into a batch
		const FilledBatch& FilledRect(
			olc::FilledBatch& batch,
			const olc::vf2d& pos,
			const olc::vf2d& size,
			const olc::Pixel colTL,
			const olc::Pixel colTR,
			const olc::Pixel colBL,
			const olc::Pixel colBR);

// === Circles ===
		
		// Draws a circle outline with a single colour
		const GPUTask& Circle(
			const olc::vf2d& pos,
			const float& radius,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws a circle outline with a single colour into a batch
		const LineBatch& Circle(
			olc::LineBatch& batch,
			const olc::vf2d& pos,
			const float& radius,
			const olc::Pixel col = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws a filled circle with a single colour
		const GPUTask& FilledCircle(
			const olc::vf2d& pos,
			const float& radius,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws a filled circle with a single colour into a batch
		const FilledBatch& FilledCircle(
			olc::FilledBatch& batch,
			const olc::vf2d& pos,
			const float& radius,
			const olc::Pixel col = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws a shaded circle with a radial gradient
		const GPUTask& FilledCircle(
			const olc::vf2d& pos,
			const float& radius,
			const olc::Pixel colInner,
			const olc::Pixel colOuter,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws a shaded circle with a radial gradient into a batch
		const FilledBatch& FilledCircle(
			olc::FilledBatch& batch,
			const olc::vf2d& pos,
			const float& radius,
			const olc::Pixel colInner,
			const olc::Pixel colOuter,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

// === Ellipses ===

		// Draws an ellipse outline with a single colour
		const GPUTask& Ellipse(
			const olc::vf2d& pos,
			const float& rx,
			const float& ry,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws an ellipse outline with a single colour into a batch
		const LineBatch& Ellipse(
			olc::LineBatch& batch,
			const olc::vf2d& pos,
			const float& rx,
			const float& ry,
			const olc::Pixel col = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws a filled ellipse with a single colour
		const GPUTask& FilledEllipse(
			const olc::vf2d& pos,
			const float& rx,
			const float& ry,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws a filled ellipse with a single colour into a batch
		const FilledBatch& FilledEllipse(
			olc::FilledBatch& batch,
			const olc::vf2d& pos,
			const float& rx,
			const float& ry,
			const olc::Pixel col = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws a shaded ellipse with a radial gradient
		const GPUTask& FilledEllipse(
			const olc::vf2d& pos,
			const float& rx,
			const float& ry,
			const olc::Pixel colInner,
			const olc::Pixel colOuter,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

		// Draws a shaded ellipse with a radial gradient into a batch
		const FilledBatch& FilledEllipse(
			olc::FilledBatch& batch,
			const olc::vf2d& pos,
			const float& rx,
			const float& ry,
			const olc::Pixel colInner,
			const olc::Pixel colOuter,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS);

// === Rounded Rectangles ===

		// Draws a rounded rectangle outline with a single colour
		const GPUTask& RoundedRect(
			const olc::vf2d& pos,						// Top left of bounding rectangle
			const olc::vf2d& size,						// Size of bounding rectangle
			const float& radius,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS / 4);

		// Draws a rounded rectangle outline with a single colour into a batch
		const LineBatch& RoundedRect(
			olc::LineBatch& batch,
			const olc::vf2d& pos,						// Top left of bounding rectangle
			const olc::vf2d& size,						// Size of bounding rectangle
			const float& radius,
			const olc::Pixel col = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS / 4);

		// Draws a filled rounded rectangle with a single colour
		const GPUTask& FilledRoundedRect(
			const olc::vf2d& pos,						// Top left of bounding rectangle
			const olc::vf2d& size,						// Size of bounding rectangle
			const float& radius,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = OLC_DEFAULT_CIRCLE_FACETS / 4);


// === Triangles ===

		// Draws a triangle outline with a single colour
		const GPUTask& Triangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a triangle outline with a single colour into a batch
		const LineBatch& Triangle(
			olc::LineBatch& batch,
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel col = olc::Colour::WHITE);

		// Draws a multiple colour triangle outline
		const GPUTask& Triangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel c1,
			const olc::Pixel c2,
			const olc::Pixel c3,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a multiple colour triangle outline into a batch
		const LineBatch& Triangle(
			olc::LineBatch& batch,
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel c1,
			const olc::Pixel c2,
			const olc::Pixel c3);

		// Draws a filled, single colour triangle
		const GPUTask& FilledTriangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled, single colour triangle into a batch
		const FilledBatch& FilledTriangle(
			olc::FilledBatch& batch,
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel col = olc::Colour::WHITE);

		// Draws a filled, multiple colour triangle
		const GPUTask& FilledTriangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel c1,
			const olc::Pixel c2,
			const olc::Pixel c3,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled, multiple colour triangle into a batch
		const FilledBatch& FilledTriangle(
			olc::FilledBatch& batch,
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel c1,
			const olc::Pixel c2,
			const olc::Pixel c3);

		// Draws a textured triangle, with per vertex colouring
		const GPUTask& TexturedTriangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel c1,
			const olc::Pixel c2,
			const olc::Pixel c3,
			const olc::vf2d& t1,
			const olc::vf2d& t2,
			const olc::vf2d& t3,
			olc::Image& texture,
			const olc::Pixel tint = olc::Colour::WHITE);

// === Polygon Outlines ===

		// Draws a polygon outline with a single colour
		const GPUTask& Polygon(
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a polygon outline with a single colour into a batch
		const LineBatch& Polygon(
			olc::LineBatch& batch,
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel col = olc::Colour::WHITE);

		// Draws a polygon outline with multiple colours
		const GPUTask& Polygon(
			const std::vector<olc::vf2d>& vecPoints,
			const std::vector<olc::Pixel>& vecColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a polygon outline with multiple colours into a batch
		const LineBatch& Polygon(
			olc::LineBatch& batch,
			const std::vector<olc::vf2d>& vecPoints,
			const std::vector<olc::Pixel>& vecColours);

	// === Structured Polygons (Outlines & Fills) ===

		// Draws a polygon outline with a single colour
		const GPUTask& Polygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel col = olc::Colour::WHITE, 
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a polygon outline with multiple colours
		const GPUTask& Polygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const std::vector<olc::Pixel>& vecColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled polygon with a single colour
		const GPUTask& FilledPolygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled polygon with multiple colours
		const GPUTask& FilledPolygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const std::vector<olc::Pixel>& vecColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a textured polygon with per vertex colouring
		const GPUTask& TexturedPolygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const std::vector<olc::Pixel>& vecColours,
			const std::vector<olc::vf2d>& vecTexCoords,
			olc::Image& texture,
			const olc::Pixel tint = olc::Colour::WHITE);


	public: // Text Drawing Functions
		// Draws a string at specified location in monospace font
		const GPUTask& String(
			const olc::vf2d& pos,
			const std::string& text, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::vf2d& scale = { 1.0f, 1.0f },
			olc::Font& font = olc::fontClassicPGE);

		// Draws a string at specified location in proportional font
		const GPUTask& StringProp(
			const olc::vf2d& pos,
			const std::string& text,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::vf2d& scale = { 1.0f, 1.0f },
			olc::Font& font = olc::fontClassicPGE);

		// Returns the bounding box size of a string in pixels
		olc::vf2d GetTextSize(
			const std::string& text,
			const bool bProportional = false,
			const olc::vf2d& scale = { 1.0f, 1.0f },
			olc::Font& font = olc::fontClassicPGE);

	
	
	public: // Image Drawing Functions		
		// Draws a scaled image at specified location
		const GPUTask& Image(
			olc::ImageRegion image, 
			const olc::vf2d& pos, 
			const olc::vf2d& scale = { 1.0f, 1.0f }, 
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a scaled image at specified location into a batch
		const ImageBatch& Image(
			olc::ImageBatch& batch,
			olc::ImageRegion image,
			const olc::vf2d& pos,
			const olc::vf2d& scale = { 1.0f, 1.0f },
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image rotated around a point at specified location
		const GPUTask& ImageRotated(
			olc::ImageRegion image, 
			const olc::vf2d& pos, 
			const float theta, 
			const olc::vf2d& center = { 0.0f, 0.0f }, 
			const olc::vf2d& scale = { 1.0f, 1.0f },
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image rotated around a point at specified location into a batch
		const ImageBatch& ImageRotated(
			olc::ImageBatch& batch,
			olc::ImageRegion image,
			const olc::vf2d& pos,
			const float theta,
			const olc::vf2d& center = { 0.0f, 0.0f },
			const olc::vf2d& scale = { 1.0f, 1.0f },
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image warped correctly to linearly fill a quadrilateral (formerly DrawWarped...)		
		const GPUTask& ImageQuad(
			olc::ImageRegion image, 
			const olc::vf2d& vTL, 
			const olc::vf2d& vTR, 
			const olc::vf2d& vBR, 
			const olc::vf2d& vBL,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image warped correctly to linearly fill a quadrilateral (formerly DrawWarped...) into a batch
		const ImageBatch& ImageQuad(
			olc::ImageBatch& batch,
			olc::ImageRegion image,
			const olc::vf2d& vTL,
			const olc::vf2d& vTR,
			const olc::vf2d& vBR,
			const olc::vf2d& vBL,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image warped correctly to linearly fill a quadrilateral (formerly DrawWarped...)
		const GPUTask& ImageQuad(
			olc::ImageRegion image, 
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image warped correctly to linearly fill a quadrilateral (formerly DrawWarped...) into a batch
		const ImageBatch& ImageQuad(
			olc::ImageBatch& batch,
			olc::ImageRegion image,
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image scaled to a specified rectangular area
		const GPUTask& ImageRect(
			olc::ImageRegion image, 
			const olc::vf2d& pos, 
			const olc::vf2d& size,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image scaled to a specified rectangular area into a batch
		const ImageBatch& ImageRect(
			olc::ImageBatch& batch,
			olc::ImageRegion image,
			const olc::vf2d& pos,
			const olc::vf2d& size,
			const olc::Pixel tint = olc::Colour::WHITE);

	public: // Applied Rendering Modes
		void SetCullMode(const olc::GPUTask::CullMode mode);
		void EnableDepth(const bool bEnable);
		void SetViewport(const olc::vi2d& pos, const olc::vi2d& size);

	public: // 3D Transformation Functions
		void MatrixReset();
		void SetModelMatrix(const olc::mf4d& mat);
		const olc::mf4d& GetModelMatrix() const;
		void SetViewMatrix(const olc::mf4d& mat);
		const olc::mf4d& GetViewMatrix() const;
		void SetProjectionMatrix(const olc::mf4d& mat);
		const olc::mf4d& GetProjectionMatrix() const;
		void SetMVPMatrix(const olc::mf4d& mat);
		const olc::mf4d& GetMVPMatrix() const;



	public: // 3D Primitive Drawing Functions

		GPUTask& Line(
			const olc::vf4d& vStart,
			const olc::vf4d& vEnd,
			const olc::Pixel& col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask& Mesh(
			const olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask& Mesh(
			const olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const std::vector<olc::vf2d>& vUVs,
			olc::Image& texture,
			const olc::Pixel tint = olc::Colour::WHITE);



	public: // GPU Task Creator Functions (not normally called by user)


	

	public:
		// Create an image batch for efficient repeated drawing of
		// the same source image
		ImageBatch CreateImageBatch(olc::Image& image);

		// Draws an image batch to the current target
		const GPUTask& Batch(olc::ImageBatch& batch, const olc::Pixel tint = olc::Colour::WHITE);

		// Create a filled shape batch for efficient repeated drawing 
		// of primitive filled shapes
		FilledBatch CreateFilledBatch();

		// Draws a filled shape batch to the current target
		const GPUTask& Batch(olc::FilledBatch& batch, const olc::Pixel tint = olc::Colour::WHITE);

		// Create a line shape batch for efficient repeated drawing
		// of primitive line shapes
		LineBatch CreateLineBatch();

		// Draws a line shape batch to the current target
		const GPUTask& Batch(olc::LineBatch& batch, const olc::Pixel tint = olc::Colour::WHITE);


	public: // GPU Task Creator Functions (not normally called by user)
		GPUTask TaskDrawLine(
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);
		
		GPUTask TaskDrawPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskDrawPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const olc::Pixel colour,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskFillPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskFillPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const olc::Pixel colour,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskTexturedPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const std::vector<olc::vf2d>& vTexCoords,
			olc::Image* const image,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskTexturedPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::vf2d>& vZWs,
			const std::vector<olc::Pixel>& vColours,
			const std::vector<olc::vf2d>& vTexCoords,
			olc::Image* const image,
			const olc::Pixel tint = olc::Colour::WHITE);

		// 3D Task Generators
		GPUTask TaskWireMesh(
			olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskFillMesh(
			olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskTexturedMesh(
			olc::Structure structure,
			const std::vector<olc::vf4d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const std::vector<olc::vf2d>& vTexCoords,
			olc::Image* const image,
			const olc::Pixel tint = olc::Colour::WHITE);


		public:
			// Change the shader used for subsequent GPU drawing tasks
			bool SetShader(const olc::gpu::Shader& shader);
			// Reset to default shader for subsequent GPU drawing tasks
			bool ResetShader();
			// Set uniform variable for subsequent GPU drawing tasks
			bool SetShaderUniform(const std::string& name, const float value);
			// Set uniform variable for subsequent GPU drawing tasks
			bool SetShaderUniform(const std::string& name, const olc::vf2d& value);
			// Set uniform variable for subsequent GPU drawing tasks
			bool SetShaderUniform(const std::string& name, const olc::Pixel value);
			// Assign an image to a texture slot for subsequent GPU drawing tasks
			bool SetShaderTexture(const uint32_t nSlot, olc::Image& image);

		public:
			struct sDrawMetrics
			{
				uint32_t nGPUTasks = 0;
				uint32_t nGPUtoCPUTransfers = 0;
				uint32_t nCPUtoGPUTransfers = 0;
				uint32_t nShaderChanges = 0;
			};

			void ResetDrawMetrics();
			sDrawMetrics GetDrawMetrics() const;

		private:
			sDrawMetrics drawMetrics;




		protected:
			// Checks residency of image resource, and brings it to cpu RAM for r/w
			void PrepareTargetForSW();
			// Checks residency of image resource, and brings it to gpu VRAM for r/w
			void PrepareTargetForHW();

			// Checks residency of image resource, and brings it to cpu RAM for r/w
			void PrepareImageForSW(olc::Image& image);
			// Checks residency of image resource, and brings it to gpu VRAM for r/w
			void PrepareImageForHW(olc::Image& image);

			olc::Image* pTarget = nullptr;
			olc::gpu::Renderer* pRenderer = nullptr;
			olc::tf2d transformAffine;

			// 3D Drawing Things
			mf4d matModel;
			mf4d matView;
			mf4d matProjection;
			mf4d matVP;
			mf4d matMVP;
			olc::vf2d vViewportPos = { 0, 0 };
			olc::vi2d vViewportSize = { 0, 0 };


			olc::GPUTask::CullMode cullMode = olc::GPUTask::CullMode::None;
			bool bDepth = true;


		private:
			// Simple dynamic buffer that only grows as needed
			template<typename T>
			struct buffer
			{
				std::vector<T> data;

				void reserve(size_t n)
				{
					if (n > data.capacity())
						data.reserve(n);

					// Ensure size matches requested so we
					// can index into it directly
					data.resize(n);
				}
			};

			// Thread local buffers to avoid repeated allocations
			static thread_local buffer<olc::vf2d> buffPoints;
			static thread_local buffer<olc::vf2d> buffUnitCirclePoints;
			static thread_local buffer<olc::Pixel> buffColours;
			static thread_local buffer<olc::GPUTask> vecGPUTasks;

			void RedefineUnitCircleBuffer(const int32_t nFacets);
	
	};
}
#define PGE_DRAW2D_DECLARED
#endif
//! END DECLARATION