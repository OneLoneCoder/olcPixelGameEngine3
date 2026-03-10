#include "draw.h"

#include "gpu_iface.h"

//! START IMPLEMENTATION
using namespace olc;

// Some local pools to reduce allocations
thread_local Draw::buffer<olc::vf2d> Draw::buffPoints;
thread_local Draw::buffer<olc::Pixel> Draw::buffColours;
thread_local Draw::buffer<olc::vf2d> Draw::buffUnitCirclePoints;
thread_local Draw::buffer<olc::GPUTask> Draw::vecGPUTasks;

Draw::Draw()
{
	vecGPUTasks.reserve(256);
}

void Draw::SetGPU(olc::gpu::Renderer* const renderer)
{
	pRenderer = renderer;
}

void Draw::SetTarget(olc::Image& image)
{
	// Perform any outstanding tasks for current target
	ProcessGPUTasks();

	// Only resolve if we're ACTUALLY changing targets
	if (pTarget && pTarget != &image && pTarget->GetConfig().MSAA)
	{
		pRenderer->ResolveMSAA(uint32_t(pTarget->GetGPUID()));
	}

	// Endure new target exists in GPU up to date
	PrepareImageForHW(image);

	// Store the target image
	pTarget = &image;	

	// Reset Affine transform to unity
	WorldReset();

	// Configure default render target
	pRenderer->AssignTextureTarget(0, uint32_t(pTarget->GetGPUID()));
	pRenderer->SetViewport({ 0,0 }, pTarget->Size());
}

olc::Image& olc::Draw::GetTarget()
{
	return *pTarget;
}

olc::vi2d olc::Draw::GetTargetSize()
{
	return pTarget->Size();
}

void olc::Draw::ProcessGPUTasks()
{
	for (const auto& task : vecGPUTasks.data)
		pRenderer->DoGPUTask(task);

	drawMetrics.nGPUTasks += uint32_t(vecGPUTasks.data.size());

	vecGPUTasks.data.clear();
}

void Draw::PrepareTargetForSW()
{
	if (pTarget->BoundToGPU())
	{
		// Process GPU queue bound for the target
		ProcessGPUTasks();

		// Image resource is primed for GPU operations, bring it to CPU
		pRenderer->ReadTexture(uint32_t(pTarget->GetGPUID()), *pTarget);

		// Image is now CPU bound
		pTarget->BindCPU();

		drawMetrics.nGPUtoCPUTransfers++;
	}
}

void Draw::PrepareTargetForHW()
{
	if (pTarget->BoundToCPU())
	{
		// Image resource is primed for CPU operations, send it to GPU
		pRenderer->WriteTexture(uint32_t(pTarget->GetGPUID()), *pTarget);

		// Image is now GPU bound
		pTarget->BindGPU();

		drawMetrics.nCPUtoGPUTransfers++;
	}
}

void Draw::PrepareImageForSW(olc::Image& image)
{
	if (image.BoundToGPU())
	{
		// Process GPU queue bound for the target
		ProcessGPUTasks();

		// Image resource is primed for GPU operations, bring it to CPU
		pRenderer->ReadTexture(uint32_t(image.GetGPUID()), image);

		// Image is now CPU bound
		image.BindCPU();

		drawMetrics.nGPUtoCPUTransfers++;
	}
}

void Draw::PrepareImageForHW(olc::Image& image)
{
	if (image.BoundToCPU())
	{
		// Image resource is primed for CPU operations, send it to GPU
		pRenderer->WriteTexture(uint32_t(image.GetGPUID()), image);

		// Image is now GPU bound
		image.BindGPU();

		drawMetrics.nCPUtoGPUTransfers++;
	}

	// Only resolve MSAA if this image is NOT the current render target
	// (i.e., we're preparing it to be SAMPLED from, not rendered to)
	if (image.GetConfig().MSAA && &image != pTarget)
	{
		pRenderer->ResolveMSAA(uint32_t(image.GetGPUID()));
	}
}

bool olc::Draw::SetShader(const olc::gpu::Shader& shader)
{
	// Finish all drawing with current shader
	ProcessGPUTasks();

	drawMetrics.nShaderChanges++;

	// Set new shader
	return pRenderer->ApplyShader(shader);
}

bool olc::Draw::ResetShader()
{
	ProcessGPUTasks();
	drawMetrics.nShaderChanges++;
	return pRenderer->ApplyDefaultShader();
}

bool olc::Draw::SetShaderUniform(const std::string& name, const float value)
{
	return pRenderer->SetUniform(name, value);
}

bool olc::Draw::SetShaderUniform(const std::string& name, const olc::vf2d& value)
{
	return pRenderer->SetUniform(name, value);	
}

bool olc::Draw::SetShaderUniform(const std::string& name, const olc::Pixel value)
{
	return pRenderer->SetUniform(name, value);
}

bool olc::Draw::SetShaderTexture(const uint32_t nSlot, olc::Image& image)
{
	PrepareImageForHW(image);
	return pRenderer->AssignTextureSource(nSlot, image.GetGPUID());	
}

void olc::Draw::ResetDrawMetrics()
{
	drawMetrics = sDrawMetrics();
}

olc::Draw::sDrawMetrics olc::Draw::GetDrawMetrics() const
{
	return drawMetrics;
}


void olc::Draw::WorldReset()
{
	transformAffine = olc::tf2d();
}

void olc::Draw::WorldScale(const olc::vf2d& vScale)
{
	transformAffine.scale(vScale);
}

void olc::Draw::WorldOffset(const olc::vf2d& vOffset)
{
	transformAffine.translate(vOffset);
}

void olc::Draw::WorldRotate(const float& fTheta, const olc::vf2d& vPoint)
{
	transformAffine.rotate(fTheta, vPoint);
}

void olc::Draw::SetWorldTransform(const olc::tf2d& trans)
{
	transformAffine = trans;
}

olc::tf2d& olc::Draw::GetWorldTransform()
{
	return transformAffine;
}

olc::vf2d olc::Draw::WorldToScreen(const olc::vf2d& v) const
{
	return transformAffine.forward(v);
}

olc::vf2d olc::Draw::ScreenToWorld(const olc::vf2d& v) const
{
	return transformAffine.inverse(v);
}

void Draw::Pixel(const olc::vf2d& pos, const olc::Pixel col, const olc::Pixel tint)
{
	// Check if in bounds
	olc::vf2d tpos = transformAffine.forwardRound(pos);
	if (tpos.x >= 0 && tpos.y >= 0 && tpos.x < float(pTarget->Size().x) && tpos.y < float(pTarget->Size().y))
	{
		PrepareTargetForSW();
		pTarget->Pixel(tpos) = col.blend(tint);
	}

	// otherwise do nothing
}

olc::Pixel olc::Draw::GetPixel(olc::Image& image, const olc::vf2d& pos)
{
	PrepareImageForSW(image);
	return image.Pixel(pos);
}

olc::Pixel olc::Draw::GetPixel(const olc::vf2d& pos)
{
	PrepareImageForSW(GetTarget());
	return GetTarget().Pixel(pos);
}

void olc::Draw::Clear(const olc::Pixel& col)
{
	PrepareTargetForHW();
	pRenderer->ClearViewport(col, true, true);
}

GPUTask olc::Draw::TaskDrawLine(const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = olc::Structure::Line;
	task.vertexBuffer.resize((vPoints.size()-1) * 2);
	for (size_t i = 0; i < vPoints.size() - 1; i++)
	{
		task.vertexBuffer[i*2+0] = { {vPoints[i].x, vPoints[i].y, 1.0f, 1.0f}, vColours[i], {0, 0}, {0, 0}, {0, 0}, {0, 0} };
		task.vertexBuffer[i*2+1] = { {vPoints[i + 1].x, vPoints[i + 1].y, 1.0f, 1.0f}, vColours[i + 1], {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	}
	task.tint = tint;
	return task;
}

GPUTask olc::Draw::TaskDrawPolygon(olc::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.bWireframe = true;
	task.vertexBuffer.resize(vPoints.size());
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, 1.0f, 1.0f}, vColours[i], {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	task.tint = tint;
	return task;
}

GPUTask olc::Draw::TaskDrawPolygon(olc::Structure structure, const std::vector<olc::vf2d>& vPoints, const olc::Pixel colour, const olc::Pixel tint)
{	
	GPUTask task;
	task.structure = structure;
	task.bWireframe = true;
	task.vertexBuffer.resize(vPoints.size());
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = {{vPoints[i].x, vPoints[i].y, 1.0f, 1.0f}, colour, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	task.tint = tint;
	return task;
}

GPUTask olc::Draw::TaskFillPolygon(olc::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.vertexBuffer.resize(vPoints.size());
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, 1.0f, 1.0f}, vColours[i], {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	task.tint = tint;
	return task;
}

GPUTask olc::Draw::TaskFillPolygon(olc::Structure structure, const std::vector<olc::vf2d>& vPoints, const olc::Pixel colour, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.vertexBuffer.resize(vPoints.size());
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, 1.0f, 1.0f}, colour, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	task.tint = tint;
	return task;	
}

GPUTask olc::Draw::TaskTexturedPolygon(olc::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const std::vector<olc::vf2d>& vTexCoords, olc::Image* const image, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.vertexBuffer.resize(vPoints.size());
	for (size_t i = 0; i<vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, 1.0f, 1.0f}, vColours[i], {vTexCoords[i].x, vTexCoords[i].y}, {0, 0}, {0, 0}, {0, 0} };
	task.pImage = image;
	task.tint = tint;
	return task;
}

GPUTask olc::Draw::TaskTexturedPolygon(olc::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::vf2d>& vZWs, const std::vector<olc::Pixel>& vColours, const std::vector<olc::vf2d>& vTexCoords, olc::Image* const image, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.vertexBuffer.resize(vPoints.size());
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, vZWs[i].x, vZWs[i].y}, vColours[i], {vTexCoords[i].x, vTexCoords[i].y}, {0, 0}, {0, 0}, {0, 0} };
	task.pImage = image;
	task.tint = tint;
	return task;
}

GPUTask olc::Draw::TaskWireMesh(olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.tint = tint;
	task.vertexBuffer.resize(vPoints.size());
	task.bWireframe = true;
	task.bDepth = bDepth;
	task.cullmode = cullMode;
	task.bIs3D = true;
	task.mvpMatrix = matMVP.m;

	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, vPoints[i].z, vPoints[i].w}, vColours[i], {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	return task;
}

GPUTask olc::Draw::TaskFillMesh(olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.tint = tint;
	task.vertexBuffer.resize(vPoints.size());
	task.bDepth = bDepth;
	task.cullmode = cullMode;
	task.bIs3D = true;
	task.mvpMatrix = matMVP.m;

	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, vPoints[i].z, vPoints[i].w}, vColours[i], {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	return task;
}

GPUTask olc::Draw::TaskTexturedMesh(olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const std::vector<olc::vf2d>& vTexCoords, olc::Image* const image, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.tint = tint;
	task.vertexBuffer.resize(vPoints.size());
	task.pImage = image;
	task.bIs3D = true;
	task.bDepth = bDepth;
	task.cullmode = cullMode;
	task.mvpMatrix = matMVP.m;
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, vPoints[i].z, vPoints[i].w}, vColours[i], {vTexCoords[i].x, vTexCoords[i].y}, {0, 0}, {0, 0}, {0, 0} };
	return task;
}



const GPUTask& Draw::Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel col, const olc::Pixel tint)
{
	PrepareTargetForHW();
	
	return vecGPUTasks.data.emplace_back(std::move(
		TaskDrawLine(
			transformAffine.forwardRoundX<float>({ p1, p2 }),			
			{ col,col },
			tint
		)));
}


const GPUTask& Draw::Line(const olc::vf2d& p1, const olc::Pixel c1, const olc::vf2d& p2, const olc::Pixel c2, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskDrawPolygon(
			olc::Structure::Line,
			transformAffine.forwardRoundX<float>({ p1, p2 }),
			{ c1, c2 },
			tint
		)));		
}


const GPUTask& olc::Draw::Rect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col, const olc::Pixel tint)
{
	// Right a big, hearty, F&^% you to OpenGL's Diamond Exit Strategy. It makes line drawing
	// with OpenGL a smidge unreliable

	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskDrawLine(
			transformAffine.forwardRoundX<float>({
				olc::vf2d(pos.x, pos.y),
				olc::vf2d(pos.x + size.x, pos.y),
				olc::vf2d(pos.x + size.x, pos.y + size.y),
				olc::vf2d(pos.x, pos.y + size.y),
				olc::vf2d(pos.x, pos.y)
				}),
			{ col, col,  col,  col,  col },
			tint
		)));
}


const GPUTask& olc::Draw::Rect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colTR, const olc::Pixel colBL, const olc::Pixel colBR, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskDrawLine(
			transformAffine.forwardRoundX<float>({
				olc::vf2d(pos.x, pos.y),
				olc::vf2d(pos.x + size.x, pos.y),
				olc::vf2d(pos.x + size.x, pos.y + size.y),
				olc::vf2d(pos.x, pos.y + size.y),
				olc::vf2d(pos.x, pos.y)
				}),
			{ colTL, colTR,  colBR,  colBL,  colTL },
			tint
				)));
}



const GPUTask& olc::Draw::FilledRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskFillPolygon(
			olc::Structure::Fan,
			transformAffine.forwardRoundX<float>({ 
				{ pos.x, pos.y }, 
				{ pos.x + size.x, pos.y }, 
				{ pos.x + size.x, pos.y + size.y },
				{ pos.x, pos.y + size.y } }),
			col,
			tint
		)));
}



const GPUTask& olc::Draw::FilledRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colTR, const olc::Pixel colBL, const olc::Pixel colBR, const olc::Pixel tint)
{
	PrepareTargetForHW();
	return vecGPUTasks.data.emplace_back(std::move(
		TaskFillPolygon(
			olc::Structure::Fan,
			transformAffine.forwardRoundX<float>({ 
				{ pos.x, pos.y }, 
				{ pos.x + size.x, pos.y }, 
				{ pos.x + size.x, pos.y + size.y }, 
				{ pos.x, pos.y + size.y } }),
			{ colTL, colTR, colBR, colBL },
			tint
		)));
}


void olc::Draw::RedefineUnitCircleBuffer(const int32_t nFacets)
{
	buffUnitCirclePoints.reserve(nFacets + 1);
	for (int32_t i = 0; i <= nFacets; i++)
	{
		float theta = float(i) / float(nFacets) * 2.0f * 3.14159265358979323846f;
		buffUnitCirclePoints.data[i] = { cosf(theta), sinf(theta) };
	}
}

const GPUTask& olc::Draw::Circle(const olc::vf2d& pos, const float& radius, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	return Ellipse(pos, radius, radius, col, tint, nFacets);
}


const GPUTask& olc::Draw::FilledCircle(const olc::vf2d& pos, const float& radius, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	return FilledEllipse(pos, radius, radius, col, tint, nFacets);
}


const GPUTask& olc::Draw::FilledCircle(const olc::vf2d& pos, const float& radius, const olc::Pixel colInner, const olc::Pixel colOuter, const olc::Pixel tint, int32_t nFacets)
{
	return FilledEllipse(pos, radius, radius, colInner, colOuter, tint, nFacets);
}


const GPUTask& olc::Draw::Ellipse(const olc::vf2d& pos, const float& rx, const float& ry, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	// Fundamental for outline circle/ellipse with colour solid/gradient
	PrepareTargetForHW();

	if (nFacets != int32_t(buffUnitCirclePoints.data.size() - 1))
		RedefineUnitCircleBuffer(nFacets);

	buffPoints.reserve(nFacets + 1);
	buffColours.reserve(nFacets + 1);

	for (int32_t i = 0; i <= nFacets; i++)
	{
		buffPoints.data[i] = transformAffine.forwardRound<float>({ pos.x + rx * buffUnitCirclePoints.data[i].x, pos.y + ry * buffUnitCirclePoints.data[i].y });
		buffColours.data[i] = col;
	}

	return vecGPUTasks.data.emplace_back(std::move(
		TaskDrawPolygon(
			olc::Structure::Line,
			buffPoints.data,
			buffColours.data,
			tint
		)));
}


const GPUTask& olc::Draw::FilledEllipse(const olc::vf2d& pos, const float& rx, const float& ry, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	return FilledEllipse(pos, rx, ry, col, col, tint, nFacets);
}


const GPUTask& olc::Draw::FilledEllipse(const olc::vf2d& pos, const float& rx, const float& ry, const olc::Pixel colInner, const olc::Pixel colOuter, const olc::Pixel tint, int32_t nFacets)
{
	// Fundamental for filled circle/ellipse with colour solid/gradient

	PrepareTargetForHW();

	if (nFacets != int32_t(buffUnitCirclePoints.data.size() - 1))
		RedefineUnitCircleBuffer(nFacets);
	
	buffPoints.reserve(nFacets + 2);
	buffColours.reserve(nFacets + 2);
	buffPoints.data[0] = transformAffine.forwardRound<float>(pos);
	buffColours.data[0] = colInner;
	
	for (int32_t i = 0; i <= nFacets; i++)
	{
		buffPoints.data[i + 1] = transformAffine.forwardRound<float>({ pos.x + rx * buffUnitCirclePoints.data[i].x, pos.y + ry * buffUnitCirclePoints.data[i].y });
		buffColours.data[i + 1] = colOuter;
	}

	return vecGPUTasks.data.emplace_back(std::move(
		TaskFillPolygon(
			olc::Structure::Fan,
			buffPoints.data,
			buffColours.data,
			tint
		)));
}


const GPUTask& olc::Draw::RoundedRect(const olc::vf2d& pos, const olc::vf2d& size, const float& radius, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	PrepareTargetForHW();

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

	buffPoints.data.push_back({ adjustedPos.x - radius, adjustedPos.y});


	return vecGPUTasks.data.emplace_back(std::move(
		TaskDrawPolygon(
			olc::Structure::Line,
			transformAffine.forwardRound<float>(buffPoints.data),
			std::vector<olc::Pixel>(buffPoints.data.size(), col),
			tint
		)));
}


const GPUTask& olc::Draw::FilledRoundedRect(const olc::vf2d& pos, const olc::vf2d& size, const float& radius, const olc::Pixel col, const olc::Pixel tint, int32_t nFacets)
{
	PrepareTargetForHW();

	buffPoints.reserve((nFacets + 1) * 4 + 1);
	buffPoints.data.clear();

	olc::vf2d adjustedPos = pos + olc::vf2d(radius, radius);
	olc::vf2d adjustedSize = size - olc::vf2d(2.0f * radius, 2.0f * radius);
	
	// Top Left
	buffPoints.data.push_back({ adjustedPos.x, adjustedPos.y });
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
	
	return vecGPUTasks.data.emplace_back(std::move(
		TaskFillPolygon(
			olc::Structure::Fan,
			transformAffine.forwardRound<float>(buffPoints.data),
			std::vector<olc::Pixel>(buffPoints.data.size(), col),
			tint
		)));
}



const GPUTask& olc::Draw::Triangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel col, const olc::Pixel tint)
{
	return Triangle(p1, p2, p3, col, col, col, tint);
}


const GPUTask& olc::Draw::Triangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskDrawPolygon(
			olc::Structure::Line,
			transformAffine.forwardRoundX<float>({ p1, p2, p3, p1 }),
			{ c1, c2, c3, c1 },
			tint
		)));
}


const GPUTask& olc::Draw::FilledTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel col, const olc::Pixel tint)
{
	return FilledTriangle(p1, p2, p3, col, col, col, tint);
}

const GPUTask& olc::Draw::FilledTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskFillPolygon(
			olc::Structure::Fan,
			transformAffine.forwardRoundX<float>({ p1, p2, p3 }),
			{ c1, c2, c3 },
			tint
		)));
}


const GPUTask& olc::Draw::TexturedTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3, const olc::vf2d& t1, const olc::vf2d& t2, const olc::vf2d& t3, olc::Image& texture, const olc::Pixel tint)
{
	PrepareTargetForHW();
	PrepareImageForHW(texture);

	return vecGPUTasks.data.emplace_back(std::move(
		TaskTexturedPolygon(
			olc::Structure::Fan,
			transformAffine.forwardRoundX<float>({ p1, p2, p3 }),
			{ c1, c2, c3 },
			{ t1, t2, t3 },
			&texture,
			tint
		)));
}

const GPUTask& olc::Draw::Polygon(const std::vector<olc::vf2d>& vecPoints, const olc::Pixel col, const olc::Pixel tint)
{
	return Polygon(olc::Structure::LineLoop, vecPoints, std::vector<olc::Pixel>(vecPoints.size(), col), tint);
}


const GPUTask& olc::Draw::Polygon(const std::vector<olc::vf2d>& vecPoints, const std::vector<olc::Pixel>& vecColours, const olc::Pixel tint)
{
	return Polygon(olc::Structure::LineLoop, vecPoints, vecColours, tint);
}


const GPUTask& olc::Draw::Polygon(const olc::Structure structure, const std::vector<olc::vf2d>& vecPoints, const olc::Pixel col, const olc::Pixel tint)
{
	return Polygon(structure, vecPoints, std::vector<olc::Pixel>(vecPoints.size(), col), tint);
}

const GPUTask& olc::Draw::Polygon(const olc::Structure structure, const std::vector<olc::vf2d>& vecPoints, const std::vector<olc::Pixel>& vecColours, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskDrawPolygon(
			structure,
			transformAffine.forwardRound<float>(vecPoints),
			vecColours,
			tint
		)));
}

const GPUTask& olc::Draw::FilledPolygon(const olc::Structure structure, const std::vector<olc::vf2d>& vecPoints, const olc::Pixel col, const olc::Pixel tint)
{
	return FilledPolygon(structure, vecPoints, std::vector<olc::Pixel>(vecPoints.size(), col), tint);
}

const GPUTask& olc::Draw::FilledPolygon(const olc::Structure structure, const std::vector<olc::vf2d>& vecPoints, const std::vector<olc::Pixel>& vecColours, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskFillPolygon(
			structure,
			transformAffine.forwardRound<float>(vecPoints),
			vecColours,
			tint
		)));
}


const GPUTask& olc::Draw::TexturedPolygon(const olc::Structure structure, const std::vector<olc::vf2d>& vecPoints, const std::vector<olc::Pixel>& vecColours, const std::vector<olc::vf2d>& vecTexCoords, olc::Image& texture, const olc::Pixel tint)
{
	PrepareTargetForHW();
	PrepareImageForHW(texture);

	return vecGPUTasks.data.emplace_back(std::move(
		TaskTexturedPolygon(
			structure,
			transformAffine.forwardRound<float>(vecPoints),
			vecColours,
			vecTexCoords,
			&texture,
			tint
		)));
}

const GPUTask& olc::Draw::String(const olc::vf2d& pos, const std::string& text, const olc::Pixel col, const olc::vf2d& scale, olc::Font& font)
{
	PrepareTargetForHW();

	olc::vf2d spos = { 0.0f, 0.0f };

	auto task = CreateImageBatch(font.imgFont);

	for (auto c : text)
	{
		const auto& glyph = font.glyphs[c];

		if (c == '\n')
		{
			spos.x = 0; 
			spos.y += font.fLineHeight * scale.y;
		}
		else if (c == '\t')
		{
			spos.x += font.fTabWidth * scale.x;
		}
		else
		{
			Draw::Image(task, font.glyphs[c].imgGlyph, pos + spos + olc::vf2d{ glyph.spacing * scale.x, 0.0f }, scale, col);
			spos.x += glyph.vMonoSize.x * scale.x;
		}
	}

	return Batch(task);
}

const GPUTask& olc::Draw::StringProp(const olc::vf2d& pos, const std::string& text, const olc::Pixel col, const olc::vf2d& scale, olc::Font& font)
{
	PrepareTargetForHW();

	olc::vf2d spos = { 0.0f, 0.0f };
	auto task = CreateImageBatch(font.imgFont);

	for (auto c : text)
	{
		const auto& glyph = font.glyphs[c];

		if (c == '\n')
		{
			spos.x = 0;
			spos.y += font.fLineHeight * scale.y;
		}
		else if (c == '\t')
		{
			spos.x += font.fTabWidth * scale.x;
		}
		else
		{
			Draw::Image(task, font.glyphs[c].imgGlyph, pos + spos, scale, col);
			spos.x += glyph.vPropSize.x * scale.x;
		}
	}

	return Batch(task);
}

olc::vf2d olc::Draw::GetTextSize(const std::string& text, const bool bProportional, const olc::vf2d& scale, olc::Font& font)
{	
	olc::vf2d size = { 0, font.fLineHeight * scale.y };
	olc::vf2d pos = { 0, font.fLineHeight * scale.y };

	for (auto c : text)
	{
		const auto& glyph = font.glyphs[c];

		if (c == '\n') 
		{ 
			pos.y += font.fLineHeight * scale.y;
			pos.x = 0; 
		}
		else if (c == '\t') 
		{ 
			pos.x += font.fTabWidth * scale.x; 
		}
		else
		{
			if (bProportional)
				pos.x += glyph.vPropSize.x * scale.x;
			else
				pos.x += glyph.vMonoSize.x * scale.x;

		}

		size = size.max(pos);
	}

	return size;	
}

GPUTask& olc::Draw::Line(const olc::vf4d& vStart, const olc::vf4d& vEnd, const olc::Pixel& col, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskWireMesh(
			olc::Structure::Line,
			{ vStart, vEnd },
			{ col, col },
			tint
		)));
}

GPUTask& olc::Draw::Mesh(const olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskWireMesh(
			structure,
			vPoints,
			vColours,
			tint
		)));
}

GPUTask& olc::Draw::Mesh(const olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const std::vector<olc::vf2d>& vUVs, olc::Image& texture, const olc::Pixel tint)
{
	PrepareImageForHW(texture);
	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskTexturedMesh(
			structure,
			vPoints,
			vColours,
			vUVs,
			&texture,
			tint
		)));
}

ImageBatch olc::Draw::CreateImageBatch(olc::Image &image)
{
	PrepareImageForHW(image);
	PrepareTargetForHW();

	ImageBatch b;
	b.task.structure = olc::Structure::List;
	b.task.pImage = &image;
	return b;
}


const GPUTask& olc::Draw::Image(olc::ImageRegion image, const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel tint)
{
	// Ensure source image is up to date in VRAM
	PrepareImageForHW(image.image);
	
	PrepareTargetForHW();

	olc::vf2d size = image.regionsize * scale;

	return vecGPUTasks.data.emplace_back(std::move(
		TaskTexturedPolygon(
			olc::Structure::Fan,
			transformAffine.forwardRoundX<float>({
				{ pos.x, pos.y }, 
				{ pos.x + size.x, pos.y }, 
				{ pos.x + size.x, pos.y + size.y }, 
				{ pos.x, pos.y + size.y } 
			}),
			{ tint, tint, tint, tint },
			// Tex coords are clockwise
			{ 
				image.coords[0],
				image.coords[1],
				image.coords[2],
				image.coords[3],
			},
			&image.image.get()
		)));

}

const GPUTask& olc::Draw::ImageRotated(olc::ImageRegion image, const olc::vf2d& pos, const float theta, const olc::vf2d& center, const olc::vf2d& scale, const olc::Pixel tint)
{
	// Ensure source image is up to date in VRAM
	PrepareImageForHW(image.image);
	PrepareTargetForHW();

	olc::vf2d size = image.regionsize * scale;

	std::vector<olc::vf2d> vPoints(4);
	vPoints[0] = olc::vf2d(0.0f, 0.0f) - (center * scale);
	vPoints[1] = olc::vf2d(size.x, 0.0f) - (center * scale);
	vPoints[2] = size - (center * scale);
	vPoints[3] = olc::vf2d(0.0f, size.y) - (center * scale);

	float c = cos(theta), s = sin(theta);
	for (size_t i = 0; i < 4; i++)
		vPoints[i] = pos + olc::vf2d(vPoints[i].x * c - vPoints[i].y * s, vPoints[i].x * s + vPoints[i].y * c);

	return vecGPUTasks.data.emplace_back(std::move(
		TaskTexturedPolygon(
			olc::Structure::Fan,
			transformAffine.forward<float>(vPoints),
			{ tint, tint, tint, tint},
			{ image.coords[0], image.coords[1], image.coords[2], image.coords[3] },
			&image.image.get()
		)));
}


const GPUTask& olc::Draw::ImageQuad(olc::ImageRegion image, const olc::vf2d& vTL, const olc::vf2d& vTR, const olc::vf2d& vBR, const olc::vf2d& vBL, const olc::Pixel tint)
{
	// Ensure source image is up to date in VRAM
	PrepareImageForHW(image.image);
	PrepareTargetForHW();


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
	
		return vecGPUTasks.data.emplace_back(std::move(
			TaskTexturedPolygon(
				olc::Structure::Fan,
				transformAffine.forwardRoundX<float>({ vTL, vTR, vBR, vBL }),
				{ {q[0], 1.0f}, {q[1], 1.0f}, {q[2], 1.0f}, {q[3], 1.0f} },
				{ tint, tint, tint, tint},
				{ image.coords[0] * q[0], image.coords[1] * q[1], image.coords[2] * q[2], image.coords[3] * q[3] },
				&image.image.get()
			)));		
		
	}

	// Default is just return a textured quad
	return vecGPUTasks.data.emplace_back(std::move(
		TaskTexturedPolygon(
			olc::Structure::Fan,
			transformAffine.forwardRoundX<float>({ vTL, vTR, vBR, vBL }),
			{ tint, tint, tint, tint },
			{ image.coords[0], image.coords[1], image.coords[2], image.coords[3] },
			&image.image.get()
		)));
}


const GPUTask& olc::Draw::ImageQuad(olc::ImageRegion image, const std::vector<olc::vf2d>& vecPoints, const olc::Pixel tint)
{
	return ImageQuad(image, vecPoints[0], vecPoints[1], vecPoints[2], vecPoints[3], tint);
}



const GPUTask& olc::Draw::ImageRect(olc::ImageRegion image, const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel tint)
{
	// Ensure source image is up to date in VRAM
	PrepareImageForHW(image.image);

	PrepareTargetForHW();

	return vecGPUTasks.data.emplace_back(std::move(
		TaskTexturedPolygon(
			olc::Structure::Fan,
			transformAffine.forwardRoundX<float>({ { pos.x, pos.y }, { pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y } }),
			{ tint, tint, tint, tint },
			// Tex coords are clockwise
			{ image.coords[0], image.coords[1], image.coords[2], image.coords[3] },
			&image.image.get()
		)));
}


void olc::Draw::SetCullMode(const olc::GPUTask::CullMode mode)
{
	cullMode = mode;
}

void olc::Draw::EnableDepth(const bool bEnable)
{
	bDepth = bEnable;
}

void olc::Draw::SetViewport(const olc::vi2d& pos, const olc::vi2d& size)
{
	pRenderer->SetViewport(pos, size);
}

void olc::Draw::MatrixReset()
{
	matMVP.identity();
	matModel.identity();
	matView.identity();
	matProjection.identity();
}

void olc::Draw::SetModelMatrix(const olc::mf4d& mat)
{
	matModel = mat;
	matMVP = matVP * matModel;
}

const olc::mf4d& olc::Draw::GetModelMatrix() const
{
	return matModel;
}

void olc::Draw::SetViewMatrix(const olc::mf4d& mat)
{
	matView = mat;
	matVP = matProjection * matView;
	matMVP = matVP * matModel;
}

const olc::mf4d& olc::Draw::GetViewMatrix() const
{
	return matView;
}

void olc::Draw::SetProjectionMatrix(const olc::mf4d& mat)
{
	matProjection = mat;
	matVP = matProjection * matView;
	matMVP = matVP * matModel;
}

const olc::mf4d& olc::Draw::GetProjectionMatrix() const
{
	return matProjection;
}

void olc::Draw::SetMVPMatrix(const olc::mf4d& mat)
{
	matMVP = mat;
}

const olc::mf4d& olc::Draw::GetMVPMatrix() const
{
	return matMVP;
}

//! END IMPLEMENTATION