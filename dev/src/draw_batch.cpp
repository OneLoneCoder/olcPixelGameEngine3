#include "draw.h"

// I've kept the implementation of batch drawing functions in a 
// separate file to avoid cluttering the main draw.cpp 

// Batching functions by neccessity treat the incoming vertices
// quite differently, decomposing into line segments or filled discrete
// triangles so the GPU only has to deal with one type of primitive 

// NOTE: There is a tremendous amount of scope for optimising these 
// functions, as they are currently very much 'brute force' implementations.

// The batch functions return a reference to the GPUTask that was created, 
// so that the call can construct the task and reuse it later. This might 
// be useful for complex geometries that need to be redrawn every frame, 
// but don't want to pay the CPU cost of reconstructing the batch every frame.

//! START IMPLEMENTATION
using namespace olc;

const GPUTask& olc::Draw::Batch(olc::ImageBatch& batch, const olc::Pixel tint)
{
	batch.task.tint = tint;
	return vecGPUTasks.data.emplace_back(batch.task);
}

FilledBatch olc::Draw::CreateFilledBatch()
{
	FilledBatch b;
	b.task.structure = olc::Structure::List;
	return b;
}

const GPUTask& olc::Draw::Batch(olc::FilledBatch& batch, const olc::Pixel tint)
{
	batch.task.tint = tint;
	return vecGPUTasks.data.emplace_back(batch.task);
}

LineBatch olc::Draw::CreateLineBatch()
{
	LineBatch b;
	b.task.structure = olc::Structure::LineList;
	return b;
}

const GPUTask& olc::Draw::Batch(olc::LineBatch& batch, const olc::Pixel tint)
{
	batch.task.tint = tint;
	return vecGPUTasks.data.emplace_back(batch.task);
}

const LineBatch& olc::Draw::Line(olc::LineBatch& batch, const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel col, const olc::Pixel tint)
{
	return Line(batch, p1, col, p2, col, tint);
}

const LineBatch& olc::Draw::Line(olc::LineBatch& batch, const olc::vf2d& p1, const olc::Pixel c1, const olc::vf2d& p2, const olc::Pixel c2, const olc::Pixel tint)
{
	batch.task.vertexBuffer.resize(batch.task.vertexBuffer.size() + 2);
	size_t idx = batch.task.vertexBuffer.size() - 2;
	const olc::vf2d a1 = transformAffine.forwardRound(p1);
	const olc::vf2d a2 = transformAffine.forwardRound(p2);
	batch.task.vertexBuffer[idx + 0] = { {a1.x, a1.y, 1.0f, 1.0f}, c1.blend(tint), {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	batch.task.vertexBuffer[idx + 1] = { {a2.x, a2.y, 1.0f, 1.0f}, c2.blend(tint), {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	return batch;
}

const LineBatch& olc::Draw::Rect(olc::LineBatch& batch, const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col, const olc::Pixel tint)
{
	return Rect(batch, pos, size, col, col, col, col, tint);
}

const LineBatch& olc::Draw::Rect(olc::LineBatch& batch, const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colTR, const olc::Pixel colBL, const olc::Pixel colBR, const olc::Pixel tint)
{
	const olc::vf2d pTL = olc::vf2d(pos.x, pos.y);
	const olc::vf2d pTR = olc::vf2d(pos.x + size.x, pos.y);
	const olc::vf2d pBR = olc::vf2d(pos.x + size.x, pos.y + size.y);
	const olc::vf2d pBL = olc::vf2d(pos.x, pos.y + size.y);

	Line(batch, pTL, colTL, pTR, colTR, tint);
	Line(batch, pTR, colTR, pBR, colBR, tint);
	Line(batch, pBR, colBR, pBL, colBL, tint);
	Line(batch, pBL, colBL, pTL, colTL, tint);

	return batch;
}

const FilledBatch& olc::Draw::FilledRect(olc::FilledBatch& batch, const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col, const olc::Pixel tint)
{
	FilledTriangle(batch, pos, olc::vf2d(pos.x + size.x, pos.y), olc::vf2d(pos.x + size.x, pos.y + size.y), col, col, col, tint);
	FilledTriangle(batch, pos, olc::vf2d(pos.x + size.x, pos.y + size.y), olc::vf2d(pos.x, pos.y + size.y), col, col, col, tint);
	return batch;
}

const FilledBatch& olc::Draw::FilledRect(olc::FilledBatch& batch, const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colTR, const olc::Pixel colBL, const olc::Pixel colBR, const olc::Pixel tint)
{
	FilledTriangle(batch, pos, olc::vf2d(pos.x + size.x, pos.y), olc::vf2d(pos.x + size.x, pos.y + size.y), colTL, colTR, colBR, tint);
	FilledTriangle(batch, pos, olc::vf2d(pos.x + size.x, pos.y + size.y), olc::vf2d(pos.x, pos.y + size.y), colTL, colBR, colBL, tint);
	return batch;
}

const LineBatch& olc::Draw::Circle(olc::LineBatch& batch, const olc::vf2d& pos, const float& radius, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	return Ellipse(batch, pos, radius, radius, col, tint, nFacets);
}

const FilledBatch& olc::Draw::FilledCircle(olc::FilledBatch& batch, const olc::vf2d& pos, const float& radius, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	return FilledEllipse(batch, pos, radius, radius, col, tint, nFacets);
}

const FilledBatch& olc::Draw::FilledCircle(olc::FilledBatch& batch, const olc::vf2d& pos, const float& radius, const olc::Pixel colInner, const olc::Pixel colOuter, const olc::Pixel tint, int32_t nFacets)
{
	return FilledEllipse(batch, pos, radius, radius, colInner, colOuter, tint, nFacets);
}

const LineBatch& olc::Draw::Ellipse(olc::LineBatch& batch, const olc::vf2d& pos, const float& rx, const float& ry, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	// Fundamental for batched outline circle/ellipse with colour solid/gradient

	if (nFacets != int32_t(buffUnitCirclePoints.data.size() - 1))
		RedefineUnitCircleBuffer(nFacets);

	for (int32_t i = 0; i <= nFacets; i++)
	{
		const olc::vf2d a1 = { buffUnitCirclePoints.data[i].x * rx + pos.x, buffUnitCirclePoints.data[i].y * ry + pos.y };
		const olc::vf2d a2 = { buffUnitCirclePoints.data[(i + 1) % buffUnitCirclePoints.data.size()].x * rx + pos.x,
			buffUnitCirclePoints.data[(i + 1) % buffUnitCirclePoints.data.size()].y * ry + pos.y };

		Line(batch, a1, col, a2, col, tint);
	}

	return batch;
}

const FilledBatch& olc::Draw::FilledEllipse(olc::FilledBatch& batch, const olc::vf2d& pos, const float& rx, const float& ry, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	return FilledEllipse(batch, pos, rx, ry, col, col, tint, nFacets);
}

const FilledBatch& olc::Draw::FilledEllipse(olc::FilledBatch& batch, const olc::vf2d& pos, const float& rx, const float& ry, const olc::Pixel colInner, const olc::Pixel colOuter, const olc::Pixel tint, int32_t nFacets)
{
	// Fundamental for batch filled circle/ellipse with colour solid/gradient

	if (nFacets != int32_t(buffUnitCirclePoints.data.size() - 1))
		RedefineUnitCircleBuffer(nFacets);

	for (int32_t i = 0; i <= nFacets; i++)
	{
		olc::vf2d p1 = { pos.x + rx * buffUnitCirclePoints.data[i].x,
						 pos.y + ry * buffUnitCirclePoints.data[i].y };

		olc::vf2d p2 = { pos.x + rx * buffUnitCirclePoints.data[(i + 1) % buffUnitCirclePoints.data.size()].x,
						 pos.y + ry * buffUnitCirclePoints.data[(i + 1) % buffUnitCirclePoints.data.size()].y };

		FilledTriangle(batch, pos, p1, p2, colInner, colOuter, colOuter, tint);
	}

	return batch;
}


const LineBatch& olc::Draw::RoundedRect(olc::LineBatch& batch, const olc::vf2d& pos, const olc::vf2d& size, const float& radius, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	buffPoints.reserve((nFacets + 1) * 4 + 1);
	buffPoints.data.clear();

	olc::vf2d adjustedPos = pos + olc::vf2d(radius, radius);
	olc::vf2d adjustedSize = size - olc::vf2d(2.0f * radius, 2.0f * radius);

	// Top Left
	for (int32_t i = 0; i <= nFacets; i++)
	{
		float theta = (float(i) / float(nFacets)) * (0.5f * 3.14159265358979323846f) - (1.0f * 3.14159265358979323846f);
		buffPoints.data.push_back({ adjustedPos.x + radius * cosf(theta), adjustedPos.y + radius * sinf(theta) });
	}

	// Top Right
	for (int32_t i = 0; i <= nFacets; i++)
	{
		float theta = (float(i) / float(nFacets)) * (0.5f * 3.14159265358979323846f) - (0.5f * 3.14159265358979323846f);
		buffPoints.data.push_back({ adjustedPos.x + adjustedSize.x + radius * cosf(theta), adjustedPos.y + radius * sinf(theta) });
	}

	// Bottom Right
	for (int32_t i = 0; i <= nFacets; i++)
	{
		float theta = (float(i) / float(nFacets)) * (0.5f * 3.14159265358979323846f) + (0.0f * 3.14159265358979323846f);
		buffPoints.data.push_back({ adjustedPos.x + adjustedSize.x + radius * cosf(theta), adjustedPos.y + adjustedSize.y + radius * sinf(theta) });
	}

	// Bottom Left
	for (int32_t i = 0; i <= nFacets; i++)
	{
		float theta = (float(i) / float(nFacets)) * (0.5f * 3.14159265358979323846f) + (0.5f * 3.14159265358979323846f);
		buffPoints.data.push_back({ adjustedPos.x + radius * cosf(theta), adjustedPos.y + adjustedSize.y + radius * sinf(theta) });
	}

	buffPoints.data.push_back({ adjustedPos.x - radius, adjustedPos.y });

	for (size_t i = 0; i < buffPoints.data.size() - 1; i++)
	{
		Line(batch, buffPoints.data[i], col, buffPoints.data[i + 1], col, tint);
	}

	return batch;
}


const LineBatch& olc::Draw::Triangle(olc::LineBatch& batch, const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel col, const olc::Pixel tint)
{
	return Triangle(batch, p1, p2, p3, col, col, col, tint);
}

const LineBatch& olc::Draw::Triangle(olc::LineBatch& batch, const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3, const olc::Pixel tint)
{
	Line(batch, p1, c1, p2, c2, tint);
	Line(batch, p2, c2, p3, c3, tint);
	Line(batch, p3, c3, p1, c1, tint);
	return batch;
}



const FilledBatch& olc::Draw::FilledTriangle(olc::FilledBatch& batch, const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel col, const olc::Pixel tint)
{
	return FilledTriangle(batch, p1, p2, p3, col, col, col, tint);
}

const FilledBatch& olc::Draw::FilledTriangle(olc::FilledBatch& batch, const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3, const olc::Pixel tint)
{
	batch.task.vertexBuffer.resize(batch.task.vertexBuffer.size() + 3);
	size_t idx = batch.task.vertexBuffer.size() - 3;
	const olc::vf2d a1 = transformAffine.forwardRound(p1);
	const olc::vf2d a2 = transformAffine.forwardRound(p2);
	const olc::vf2d a3 = transformAffine.forwardRound(p3);
	batch.task.vertexBuffer[idx + 0] = { {a1.x, a1.y, 1.0f, 1.0f}, c1.blend(tint), {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	batch.task.vertexBuffer[idx + 1] = { {a2.x, a2.y, 1.0f, 1.0f}, c2.blend(tint), {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	batch.task.vertexBuffer[idx + 2] = { {a3.x, a3.y, 1.0f, 1.0f}, c3.blend(tint), {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	return batch;
}


const LineBatch& olc::Draw::Polygon(olc::LineBatch& batch, const std::vector<olc::vf2d>& vecPoints, const olc::Pixel col, const olc::Pixel tint)
{
	for (size_t i = 0; i < vecPoints.size(); i++)
	{
		Line(batch, vecPoints[i], col, vecPoints[(i + 1) % vecPoints.size()], col, tint);
	}

	return batch;
}

const LineBatch& olc::Draw::Polygon(olc::LineBatch& batch, const std::vector<olc::vf2d>& vecPoints, const std::vector<olc::Pixel>& vecColours, const olc::Pixel tint)
{
	for (size_t i = 0; i < vecPoints.size(); i++)
	{
		Line(batch, vecPoints[i], vecColours[i], vecPoints[(i + 1) % vecPoints.size()], vecColours[(i + 1) % vecColours.size()], tint);
	}

	return batch;
}

const FilledBatch& olc::Draw::FilledPolygon(FilledBatch& batch, const olc::Structure structure, const std::vector<olc::vf2d>& vecPoints, const std::vector<olc::Pixel>& vecColours, const olc::Pixel tint)
{
	// TODO: Curiously, this approach is considerably slower than the naive approach of just 
	// calling FilledTriangle for each triangle in the polygon. I suspect this is due to the 
	// overhead of copying verts into the temporary buffer and then into the batch buffer, 
	// but it is worth investigating further.
	//
	// The challenge here is olc::Structure changes. Perhaps its worth flushing the batch
	// when the structure changes, but this would prevent retaining composites for future
	// reuse.

	auto pushTriangle = [&](const size_t idx, const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel& c1, const olc::Pixel& c2, const olc::Pixel& c3)
		{
			batch.task.vertexBuffer[idx + 0] = { {p1.x, p1.y, 1.0f, 1.0f}, c1, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
			batch.task.vertexBuffer[idx + 1] = { {p2.x, p2.y, 1.0f, 1.0f}, c2, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
			batch.task.vertexBuffer[idx + 2] = { {p3.x, p3.y, 1.0f, 1.0f}, c3, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
		};

	// Transform unique verts into temporary buffer
	buffPoints.data.clear();
	buffColours.data.clear();
	buffPoints.reserve(vecPoints.size());
	buffColours.reserve(vecColours.size());
	for (size_t i = 0; i < vecPoints.size(); i++)
	{
		buffPoints.data[i] = transformAffine.forwardRound<float>(vecPoints[i]);
		buffColours.data[i] = vecColours[i].blend(tint);
	}

	switch (structure)
	{
	case olc::Structure::Fan:
	{
		size_t idx = batch.task.vertexBuffer.size();
		batch.task.vertexBuffer.resize(batch.task.vertexBuffer.size() + (vecPoints.size() - 2) * 3);

		for (size_t i = 1; i < vecPoints.size() - 1; i++)
			pushTriangle(idx + (i - 1) * 3,
				buffPoints.data[0], buffPoints.data[i], buffPoints.data[i + 1],
				buffColours.data[0], buffColours.data[i], buffColours.data[i + 1]);
	}
	break;

	case olc::Structure::Strip:
	{
		size_t idx = batch.task.vertexBuffer.size();
		batch.task.vertexBuffer.resize(batch.task.vertexBuffer.size() + (vecPoints.size() - 2) * 3);

		for (size_t i = 0; i < vecPoints.size() - 2; i++)
			pushTriangle(idx + (i * 3),
				buffPoints.data[i], buffPoints.data[i + 1], buffPoints.data[i + 2],
				buffColours.data[i], buffColours.data[i + 1], buffColours.data[i + 2]);
	}
	break;

	case olc::Structure::List:
	{
		size_t idx = batch.task.vertexBuffer.size();
		batch.task.vertexBuffer.resize(batch.task.vertexBuffer.size() + (vecPoints.size() / 3));

		for (size_t i = 0; i < vecPoints.size(); i += 3)
			pushTriangle(idx + i,
				buffPoints.data[i], buffPoints.data[i + 1], buffPoints.data[i + 2],
				buffColours.data[i], buffColours.data[i + 1], buffColours.data[i + 2]);
	}
	}

	return batch;
}



const ImageBatch& olc::Draw::Image(ImageBatch& batch, olc::ImageRegion image, const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel tint)
{
	// Add quad to existing task
	olc::vf2d size = image.regionsize * scale;

	olc::vf2d p0 = transformAffine.forward(olc::vf2d{ pos.x, pos.y });
	olc::vf2d p1 = transformAffine.forward(olc::vf2d{ pos.x + size.x, pos.y });
	olc::vf2d p2 = transformAffine.forward(olc::vf2d{ pos.x + size.x, pos.y + size.y });
	olc::vf2d p3 = transformAffine.forward(olc::vf2d{ pos.x, pos.y + size.y });

	//batch.task.vertexBuffer.reserve(batch.task.vertexBuffer.size() + 6); // NOTE!! This tanked performance on large batches
	// NOTE: We fake tint by simply setting vertex colour
	batch.task.vertexBuffer.push_back({ {p0.x, p0.y, 1.0f, 1.0f}, tint, {image.coords[0].x, image.coords[0].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p1.x, p1.y, 1.0f, 1.0f}, tint, {image.coords[1].x, image.coords[1].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p2.x, p2.y, 1.0f, 1.0f}, tint, {image.coords[2].x, image.coords[2].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p0.x, p0.y, 1.0f, 1.0f}, tint, {image.coords[0].x, image.coords[0].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p2.x, p2.y, 1.0f, 1.0f}, tint, {image.coords[2].x, image.coords[2].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p3.x, p3.y, 1.0f, 1.0f}, tint, {image.coords[3].x, image.coords[3].y}, {0, 0}, {0, 0}, {0, 0} });
	return batch;
}

const ImageBatch& olc::Draw::ImageRotated(olc::ImageBatch& batch, olc::ImageRegion image, const olc::vf2d& pos, const float theta, const olc::vf2d& center, const olc::vf2d& scale, const olc::Pixel tint)
{
	// Add quad to existing task
	olc::vf2d size = image.regionsize;

	std::array<olc::vf2d, 4> vPoints;
	vPoints[0] = (olc::vf2d(0.0f, 0.0f) - center) * scale;
	vPoints[1] = (olc::vf2d(size.x, 0.0f) - center) * scale;
	vPoints[2] = (size - center) * scale;
	vPoints[3] = (olc::vf2d(0.0f, size.y) - center) * scale;

	float c = cos(theta), s = sin(theta);
	for (size_t i = 0; i < 4; i++)
		vPoints[i] = pos + olc::vf2d(vPoints[i].x * c - vPoints[i].y * s, vPoints[i].x * s + vPoints[i].y * c);

	olc::vf2d p0 = transformAffine.forward(vPoints[0]);
	olc::vf2d p1 = transformAffine.forward(vPoints[1]);
	olc::vf2d p2 = transformAffine.forward(vPoints[2]);
	olc::vf2d p3 = transformAffine.forward(vPoints[3]);

	//batch.task.vertexBuffer.reserve(batch.task.vertexBuffer.size() + 6);
	batch.task.vertexBuffer.push_back({ {p0.x, p0.y, 1.0f, 1.0f}, tint, {image.coords[0].x, image.coords[0].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p1.x, p1.y, 1.0f, 1.0f}, tint, {image.coords[1].x, image.coords[1].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p2.x, p2.y, 1.0f, 1.0f}, tint, {image.coords[2].x, image.coords[2].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p0.x, p0.y, 1.0f, 1.0f}, tint, {image.coords[0].x, image.coords[0].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p2.x, p2.y, 1.0f, 1.0f}, tint, {image.coords[2].x, image.coords[2].y}, {0, 0}, {0, 0}, {0, 0} });
	batch.task.vertexBuffer.push_back({ {p3.x, p3.y, 1.0f, 1.0f}, tint, {image.coords[3].x, image.coords[3].y}, {0, 0}, {0, 0}, {0, 0} });
	return batch;
}

const ImageBatch& olc::Draw::ImageQuad(olc::ImageBatch& batch, olc::ImageRegion image, const olc::vf2d& vTL, const olc::vf2d& vTR, const olc::vf2d& vBR, const olc::vf2d& vBL, const olc::Pixel tint)
{
	float rd = ((vBR.x - vTL.x) * (vTR.y - vBL.y) - (vTR.x - vBL.x) * (vBR.y - vTL.y));
	if (rd != 0)
	{
		rd = 1.0f / rd;
		float rn = ((vTR.x - vBL.x) * (vTL.y - vBL.y) - (vTR.y - vBL.y) * (vTL.x - vBL.x)) * rd;
		float sn = ((vBR.x - vTL.x) * (vTL.y - vBL.y) - (vBR.y - vTL.y) * (vTL.x - vBL.x)) * rd;

		olc::vf2d center;
		if (!(rn < 0.f || rn > 1.f || sn < 0.f || sn > 1.f))
			center = vTL + rn * (vBR - vTL);

		std::array<float, 4> d = { {
			(vTL - center).mag(),
			(vTR - center).mag(),
			(vBR - center).mag(),
			(vBL - center).mag(),
		} };

		std::array<float, 4> q = { {
			d[0] == 0.0f ? 1.0f : (d[0] + d[2]) / d[2],
			d[1] == 0.0f ? 1.0f : (d[1] + d[3]) / d[3],
			d[2] == 0.0f ? 1.0f : (d[2] + d[0]) / d[0],
			d[3] == 0.0f ? 1.0f : (d[3] + d[1]) / d[1],
		} };

		olc::vf2d p0 = transformAffine.forward(vTL);
		olc::vf2d p1 = transformAffine.forward(vTR);
		olc::vf2d p2 = transformAffine.forward(vBR);
		olc::vf2d p3 = transformAffine.forward(vBL);

		//batch.task.vertexBuffer.reserve(batch.task.vertexBuffer.size() + 6);
		batch.task.vertexBuffer.push_back({ {p0.x, p0.y, q[0], 1.0f}, tint, {q[0] * image.coords[0].x, q[0] * image.coords[0].y}, {0, 0}, {0, 0}, {0, 0} });
		batch.task.vertexBuffer.push_back({ {p1.x, p1.y, q[1], 1.0f}, tint, {q[1] * image.coords[1].x, q[1] * image.coords[1].y}, {0, 0}, {0, 0}, {0, 0} });
		batch.task.vertexBuffer.push_back({ {p2.x, p2.y, q[2], 1.0f}, tint, {q[2] * image.coords[2].x, q[2] * image.coords[2].y}, {0, 0}, {0, 0}, {0, 0} });
		batch.task.vertexBuffer.push_back({ {p0.x, p0.y, q[0], 1.0f}, tint, {q[0] * image.coords[0].x, q[0] * image.coords[0].y}, {0, 0}, {0, 0}, {0, 0} });
		batch.task.vertexBuffer.push_back({ {p2.x, p2.y, q[2], 1.0f}, tint, {q[2] * image.coords[2].x, q[2] * image.coords[2].y}, {0, 0}, {0, 0}, {0, 0} });
		batch.task.vertexBuffer.push_back({ {p3.x, p3.y, q[3], 1.0f}, tint, {q[3] * image.coords[3].x, q[3] * image.coords[3].y}, {0, 0}, {0, 0}, {0, 0} });
		return batch;
	}

	// Default is just return a textured quad
	return Draw::Image(batch, image, vTL, vBR - vTL, tint);
}

const ImageBatch& olc::Draw::ImageQuad(olc::ImageBatch& batch, olc::ImageRegion image, const std::vector<olc::vf2d>& vecPoints, const olc::Pixel tint)
{
	return ImageQuad(batch, image, vecPoints[0], vecPoints[1], vecPoints[2], vecPoints[3], tint);
}

const ImageBatch& olc::Draw::ImageRect(olc::ImageBatch& batch, olc::ImageRegion image, const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel tint)
{
	olc_IgnoreUnused(image, pos, size, tint);
	// TODO: Implement this function
	return batch;
}




//! END IMPLEMENTATION