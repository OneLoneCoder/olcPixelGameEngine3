#include "draw2d.h"

#include "gpu_iface.h"



//! START IMPLEMENTATION

// This is all essentially the olc::PixelGameEngine 2 rasteriser code
using namespace olc;


void Draw2D::swLine(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel col)
{
	swLine(p1, p2, col, col);
}

void Draw2D::swLine(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel c1, const olc::Pixel c2)
{
	const auto vTransformedPoints = transformAffine.forward<float>({ p1, p2 });
	swRasterShadedLine(
		vTransformedPoints[0],
		vTransformedPoints[1],
		c1, c2);	
}

void olc::Draw2D::swRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col)
{
	swRect(pos, size, col, col, col, col);
}

void olc::Draw2D::swRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colTR, const olc::Pixel colBL, const olc::Pixel colBR)
{
	swLine(pos, { pos.x + size.x, pos.y }, colTL, colTR);
	swLine({ pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, colTR, colBR);
	swLine({ pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y }, colBR, colBL);
	swLine({ pos.x, pos.y + size.y }, pos, colBL, colTL);
}

void olc::Draw2D::swFilledRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col)
{
	swFilledRect(pos, size, col, col, col, col);
}

void olc::Draw2D::swFilledRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colTR, const olc::Pixel colBL, const olc::Pixel colBR)
{
	const auto vTransformedPoints = transformAffine.forward<float>({pos, {pos.x + size.x, pos.y}, pos + size, {pos.x, pos.y + size.y}});
	
	// Most draws will be single colour, axis aligned rectangle. 
	// Optimise for that case first

	// Check if all one colour
	if (colTL == colBL && colTL == colTR && colTL == colBR)
	{
		// Check if axis aligned
		if(vTransformedPoints[0].y == vTransformedPoints[1].y &&
		   vTransformedPoints[1].x == vTransformedPoints[2].x &&
		   vTransformedPoints[2].y == vTransformedPoints[3].y &&
		   vTransformedPoints[3].x == vTransformedPoints[0].x)
		{
			PrepareTargetForSW();

			// Clip to target
			olc::vi2d p1 = vTransformedPoints[0].max({ 0,0 });
			olc::vi2d p2 = vTransformedPoints[2].min(pTarget->Size());
			
			// Draw filled rectangle
			for (int32_t y = p1.y; y < p2.y; y++)
				for (int32_t x = p1.x; x < p2.x; x++)
					pTarget->Pixel({ x, y }) = colTL;

			// Exit early
			return;
		}		
	}

	// Fallback to general case rasteriser, where we split into two triangles
	swRasterShadedTriangle(
		vTransformedPoints[0],
		vTransformedPoints[1],
		vTransformedPoints[2],
		colTL, colTR, colBR);
	swRasterShadedTriangle(
		vTransformedPoints[0],
		vTransformedPoints[2],
		vTransformedPoints[3],
		colTL, colBR, colBL);			
}

void olc::Draw2D::swTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel col)
{
	swTriangle(p1, p2, p3, col, col, col);
}

void olc::Draw2D::swTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3)
{
	swLine(p1, p2, c1, c2);
	swLine(p2, p3, c2, c3);
	swLine(p3, p1, c3, c1);
}

void olc::Draw2D::swFilledTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel col)
{
	swFilledTriangle(p1, p2, p3, col, col, col);
}

void olc::Draw2D::swFilledTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3)
{
	const auto vTransformedPoints = transformAffine.forward<float>({ p1, p2, p3 });
	swRasterShadedTriangle(
		vTransformedPoints[0],
		vTransformedPoints[1],
		vTransformedPoints[2],
		c1, c2, c3);
}

void olc::Draw2D::swTexturedTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3, const olc::vf2d& t1, const olc::vf2d& t2, const olc::vf2d& t3, olc::Image& texture)
{
	const auto vTransformedPoints = transformAffine.forward<float>({ p1, p2, p3 });
	swRasterTexturedTriangle(
		vTransformedPoints[0],
		vTransformedPoints[1],
		vTransformedPoints[2],
		c1, c2, c3, 
		t1, t2, t3, 
		texture);
}

bool olc::Draw2D::swClipLine(olc::vf2d& p1, olc::vf2d& p2, const olc::vf2d& vMin, const olc::vf2d& vMax)
{
	// https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
	static constexpr int SEG_I = 0b0000, SEG_L = 0b0001, SEG_R = 0b0010, SEG_B = 0b0100, SEG_T = 0b1000;
	auto Segment = [&vMin = vMin, &vMax = vMax](const olc::vi2d& v)
		{
			int i = SEG_I;
			if (v.x < vMin.x) i |= SEG_L; else if (v.x > vMax.x) i |= SEG_R;
			if (v.y < vMin.y) i |= SEG_B; else if (v.y > vMax.y) i |= SEG_T;
			return i;
		};

	int s1 = Segment(p1), s2 = Segment(p2);

	while (true)
	{
		if (!(s1 | s2))	  return true;
		else if (s1 & s2) return false;
		else
		{
			int s3 = s2 > s1 ? s2 : s1;
			olc::vf2d n;
			if (s3 & SEG_T) { n.x = p1.x + (p2.x - p1.x) * (vMax.y - p1.y) / (p2.y - p1.y); n.y = vMax.y; }
			else if (s3 & SEG_B) { n.x = p1.x + (p2.x - p1.x) * (vMin.y - p1.y) / (p2.y - p1.y); n.y = vMin.y; }
			else if (s3 & SEG_R) { n.x = vMax.x; n.y = p1.y + (p2.y - p1.y) * (vMax.x - p1.x) / (p2.x - p1.x); }
			else if (s3 & SEG_L) { n.x = vMin.x; n.y = p1.y + (p2.y - p1.y) * (vMin.x - p1.x) / (p2.x - p1.x); }
			if (s3 == s1) { p1 = n; s1 = Segment(p1); }
			else { p2 = n; s2 = Segment(p2); }
		}
	}
	return true;
}

bool olc::Draw2D::swClipWeightedLine(olc::vf2d& v0, olc::vf2d& v1, const olc::vf2d& vMin, const olc::vf2d& vMax, float& w0, float& w1)
{
	// Liang-Barsky line clipping algorithm adapted for weighted lines
	// https://en.wikipedia.org/wiki/Liang%E2%80%93Barsky_algorithm
	
	olc::vf2d diff = v1 - v0;	

	float p[4] = { -diff.x, diff.x, -diff.y, diff.y };

	float q[4] = 
	{
		v0.x - vMin.x,
		vMax.x - v0.x, 
		v0.y - vMin.y, 
		vMax.y - v0.y 
	};
	
	// Weights are ideal to start, we'll contact them as we clip
	w0 = 0.0f;	
	w1 = 1.0f;

	for (int i = 0; i < 4; i++)
	{
		if (p[i] == 0.0f)
		{
			if (q[i] < 0.0f)
				return false; // Line is parallel and outside the clipping boundary
		}
		else
		{
			float t = q[i] / p[i];
			if (p[i] < 0.0f)
			{
				if (t > w1)
					return false; // Line is outside the clipping boundary
				else if (t > w0)
					w0 = t;
			}
			else
			{
				if (t < w0)
					return false; // Line is outside the clipping boundary
				else if (t < w1)
					w1 = t; 
			}
		}
	}

	if (w1 < w0)
		return false; // Line is outside the clipping boundary

	// Return new line segment ends
	v0 = v0 + diff * w0;
	v1 = v1 + diff * w1;

	// Line has visible pixels inside clipping boundary
	return true;
}

std::pair<int, int> olc::Draw2D::swBaryFillTriangle(const olc::vi2d& v1, const olc::vi2d& v2, const olc::vi2d& v3)
{
	// Get height of triangle in whole pixels
	int32_t nMinY = std::min({ v1.y, v2.y, v3.y });
	int32_t nMaxY = std::max({ v1.y, v2.y, v3.y });
	int32_t nHeight = nMaxY - nMinY;

	if (nHeight <= 0)
		return { 0, 0 }; // Degenerate triangle

	// Scanline buffer is already allocated to be the max vertical size
	// of the draw target. Obviously it only represents visible scanlines
	// that are to be filled for the current triangle.

	// Get visible height of triangle
	int32_t y_min = std::max(0, nMinY);
	int32_t y_max = std::min(nMaxY, pTarget->Size().y);

	// Zero out scanline buffer (by resetting min and max values)
	for (int32_t y = y_min; y < y_max; y++)
	{
		vScanlines[y].nMin = std::numeric_limits<int32_t>::max();
		vScanlines[y].nMax = std::numeric_limits<int32_t>::min();
	}

	// This function scans an edge of the triangle, updating
	// the scanline buffer with min/max extents and barycentric coords.
	// It returns the number of scanlines updated.
	auto scanEdge = [&](olc::vi2d p0, olc::vi2d p1, int id1, int id2) -> size_t
		{
			if (p0.y == p1.y)
				return 0;

			// Ensure p0.y < p1.y
			bool swapped = false;
			if (p0.y > p1.y)
			{
				std::swap(p0, p1);
				swapped = true;
			}

			// Cache edge step deltas
			int dy = p1.y - p0.y;
			float dx_step = (p1.x - p0.x) / float(dy);
			float dy_step = 1.0f / float(dy);
			float x = p0.x;

			// Rasterise edge - if pixel lies on visible scanline then
			// update the scanline bounds and barycentric coords
			size_t nScanline = 0;
			for (int y = p0.y; y <= p1.y; y++)
			{
				// If this pixel row is visible
				if (y >= 0 && y < vScanlines.size())
				{
					int ix = int(std::round(x));

					// interpolation along edge 
					// Note: We may need to do this differently when clipping
					float t = (y - p0.y) * dy_step;

					std::array<float, 3> bary = { 0.0f, 0.0f, 0.0f };

					// Set barycentric coords depending on edge direction
					if (swapped)
					{
						bary[id1] = t;
						bary[id2] = 1.0f - t;
					}
					else
					{
						bary[id1] = 1.0f - t;
						bary[id2] = t;
					}

					// Update scanline extents and barycentric coords
					if (ix < vScanlines[y].nMin)
					{
						vScanlines[y].nMin = ix;
						vScanlines[y].fBaryMin = bary;
					}

					if (ix > vScanlines[y].nMax)
					{
						vScanlines[y].nMax = ix;
						vScanlines[y].fBaryMax = bary;
					}

					nScanline++;
				}

				x += dx_step;
			}

			return nScanline;
		};

	// Rasterise triangle edges into scanline buffer
	scanEdge(v1, v2, 0, 1);
	scanEdge(v1, v3, 0, 2);
	scanEdge(v2, v3, 1, 2);

	return { y_min, y_max };
}

void olc::Draw2D::swRasterShadedTriangle(const olc::vi2d& v1, const olc::vi2d& v2, const olc::vi2d& v3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3)
{
	// We are writing to the target image, so make sure its memory resident (and up to date)
	PrepareTargetForSW();

	auto [y_min, y_max] = swBaryFillTriangle(v1, v2, v3);

	// Now draw the scanlines
	for (int32_t y = y_min; y < y_max; y++)
	{
		const auto& scanline = vScanlines[y];

		int32_t xStart = scanline.nMin;
		int32_t xEnd = scanline.nMax;

		int32_t x_min = std::max(0, xStart);
		int32_t x_max = std::min(xEnd, pTarget->Size().x);

		float fSpan = float(xEnd - xStart);
		float fSpanStep = fSpan > 0.0f ? 1.0f / fSpan : 0.0f;

		float b0_step = fSpanStep * (scanline.fBaryMax[0] - scanline.fBaryMin[0]);
		float b1_step = fSpanStep * (scanline.fBaryMax[1] - scanline.fBaryMin[1]);
		float b2_step = fSpanStep * (scanline.fBaryMax[2] - scanline.fBaryMin[2]);

		float b0 = scanline.fBaryMin[0];
		float b1 = scanline.fBaryMin[1];
		float b2 = scanline.fBaryMin[2];

		if (xStart < 0)
		{
			b0 = scanline.fBaryMin[0] + (-xStart * b0_step);
			b1 = scanline.fBaryMin[1] + (-xStart * b1_step);
			b2 = scanline.fBaryMin[2] + (-xStart * b2_step);
		}

		for (int32_t x = x_min; x < x_max; x++)
		{
			olc::Pixel col = olc::Pixel(
				uint8_t(c1.r * b0 + c2.r * b1 + c3.r * b2),
				uint8_t(c1.g * b0 + c2.g * b1 + c3.g * b2),
				uint8_t(c1.b * b0 + c2.b * b1 + c3.b * b2),
				uint8_t(c1.a * b0 + c2.a * b1 + c3.a * b2));

			// In theory, target (x,y) is always valid here due to clipping above
			pTarget->Pixel({ x, y }) = col;

			b0 += b0_step;
			b1 += b1_step;
			b2 += b2_step;
		}
	}


	return;
}

void olc::Draw2D::swRasterTexturedTriangle(const olc::vi2d& v1, const olc::vi2d& v2, const olc::vi2d& v3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3, const olc::vf2d& t1, const olc::vf2d& t2, const olc::vf2d& t3, olc::Image& texture)
{
	// We are writing to the target image, so make sure its memory resident (and up to date)
	PrepareTargetForSW();
	PrepareImageForSW(texture);

	auto [y_min, y_max] = swBaryFillTriangle(v1, v2, v3);

	// Now draw the scanlines
	for (int32_t y = y_min; y < y_max; y++)
	{
		const auto& scanline = vScanlines[y];

		int32_t xStart = scanline.nMin;
		int32_t xEnd = scanline.nMax;

		int32_t x_min = std::max(0, xStart);
		int32_t x_max = std::min(xEnd, pTarget->Size().x);

		float fSpan = float(xEnd - xStart);
		float fSpanStep = fSpan > 0.0f ? 1.0f / fSpan : 0.0f;
		
		float b0_step = fSpanStep * (scanline.fBaryMax[0] - scanline.fBaryMin[0]);
		float b1_step = fSpanStep * (scanline.fBaryMax[1] - scanline.fBaryMin[1]);
		float b2_step = fSpanStep * (scanline.fBaryMax[2] - scanline.fBaryMin[2]);

		float b0 = scanline.fBaryMin[0];
		float b1 = scanline.fBaryMin[1];
		float b2 = scanline.fBaryMin[2];

		if(xStart < 0)
		{
			b0 = scanline.fBaryMin[0] + (-xStart * b0_step);
			b1 = scanline.fBaryMin[1] + (-xStart * b1_step);
			b2 = scanline.fBaryMin[2] + (-xStart * b2_step);
		}

		for (int32_t x = x_min; x < x_max; x++)
		{
			olc::Pixel col = olc::Pixel(
				uint8_t(c1.r * b0 + c2.r * b1 + c3.r * b2),
				uint8_t(c1.g * b0 + c2.g * b1 + c3.g * b2),
				uint8_t(c1.b * b0 + c2.b * b1 + c3.b * b2),
				uint8_t(c1.a * b0 + c2.a * b1 + c3.a * b2));

			olc::vf2d uv = olc::vf2d(
				b0 * t1.x + b1 * t2.x + b2 * t3.x,
				b0 * t1.y + b1 * t2.y + b2 * t3.y);

			
			// In theory, target (x,y) is always valid here due to clipping above
			pTarget->Pixel({ x, y }) = col.blend(texture.Sample(uv));
	
			b0 += b0_step;
			b1 += b1_step;
			b2 += b2_step;
		}
	}

	return;
}

void olc::Draw2D::swRasterShadedLine(const olc::vi2d& v1, const olc::vi2d& v2, const olc::Pixel c1, const olc::Pixel c2)
{
	PrepareTargetForSW();

	// Lambda to draw a pixel gated by a pattern bit
	uint32_t pattern = 0xFFFFFFFF;
	auto rol = [&](void)
		{
			pattern = (pattern << 1) | (pattern >> 31);
			return pattern & 1;
		};

	// Lambda to draw a pixel at integer location
	auto Plot = [&](int32_t x, int32_t y, const olc::Pixel& p)
		{
			if (x >= 0 && x < pTarget->Size().x && y >= 0 && y < pTarget->Size().y)
				pTarget->Pixel({ x, y }) = p;
		};

	// Clip line to draw target
	olc::vf2d clipped_p1 = v1;
	olc::vf2d clipped_p2 = v2;

	// If line is completely outside bounds, exit
	//if (!swClipLine(clipped_p1, clipped_p2, { 0,0 }, pTarget->Size()))
//		return;

	float w0, w1;
	if (!swClipWeightedLine(clipped_p1, clipped_p2, { 0,0 }, pTarget->Size(), w0, w1))
		return;

	// Move to integer space
	olc::vi2d ip1 =  clipped_p1.round();// .floor();
	olc::vi2d ip2 =  clipped_p2.round();// .floor();
	olc::vi2d pixel;

	// Calculate deltas
	int dx = ip2.x - ip1.x;
	int dy = ip2.y - ip1.y;
	int absDx = std::abs(dx);
	int absDy = std::abs(dy);

	// Determine dominant axis
	bool xMajor = absDx >= absDy;
	int steps = xMajor ? absDx : absDy;

	// Handle degenerate case (single pixel)
	if (steps == 0)
	{
		Plot(ip1.x, ip1.y, c1);
		return;
	}

	// Calculate step increments
	float xStep = float(dx) / float(steps);
	float yStep = float(dy) / float(steps);
	float colorStep = 1.0f / float(steps);

	// Starting position and color interpolation parameter
	float x = ip1.x;
	float y = ip1.y;
	float t = 0.0f;

	// Draw line pixel by pixel
	for (int i = 0; i <= steps; i++)
	{
		// Interpolate color
		olc::Pixel col = olc::PixelLerp(c1, c2, t);

		// Plot pixel
		if(rol())
			Plot((int)std::round(x), (int)std::round(y), col);

		// Step to next pixel
		x += xStep;
		y += yStep;
		t += colorStep;
	}



	return;
}



//! END IMPLEMENTATION