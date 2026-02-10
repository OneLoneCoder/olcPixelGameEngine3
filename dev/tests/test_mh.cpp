#define OLC_PGE3_APPLICATION
#include "olcpge3.h"


class SecondWindow : public olc::PGEWindow
{
public:

	// Return true if window is to continue
	bool OnUserCreate() override
	{

		return false;
	}

	// Return true if window is to continue
	bool OnUserUpdate(float fElapsedTime) override
	{
		fTotalTime += fElapsedTime;

		draw.Clear(olc::Colour::RED);
		draw.WorldRotate(fTotalTime, GetScreen().Size() / 2);
		draw.FilledRect({ 10,10 }, { 20,20 }, olc::Colour::BLUE);
		return true;
	}


	float fTotalTime = 0;
};


class Example : public olc::PixelGameEngine
{
public:
	Example()
	{

	}

	olc::Image imTest;

	olc::Image imLogo;
	olc::Image imTemp;

	olc::Image imSampleTest;

	olc::Image imLowRes;
	olc::Image imHighResSprite;

	void CreateSampleTestImage(olc::Image& image, const olc::vi2d& s)
	{
		CreateImage(image, s);
		draw.SetTarget(image);

		draw.Clear(olc::Colour::WHITE);

		std::vector<olc::Pixel> vColours = {
			olc::Colour::RED,
			olc::Colour::YELLOW,
			olc::Colour::GREEN,
			olc::Colour::CYAN,
			olc::Colour::BLUE,
			olc::Colour::MAGENTA,
		};

		for (int i = 0; i < s.x/2; i++)
		{
			draw.Rect(olc::vi2d{ i,i }, s - olc::vi2d{ (i * 2)+1 ,(i * 2)+1  },  vColours[i % vColours.size()]);
		}

		draw.Pixel({ 0.0,1 }, olc::Colour::BLACK);

	}

	float fAngle = 0.0f;


	struct logo
	{
		olc::vf2d pos;
		olc::vf2d vel;
		float ang = 0;
		float angvel = 0;
	};
	std::vector<logo> vecLogos;

	std::vector<olc::vf2d> vecVerts;

	float fScale = 1.0f;


	int nSelectedVert = -1;

	std::shared_ptr<SecondWindow> win2;

	olc::Image imBlend;
	olc::Image imTempBuffer;

	std::vector<olc::vf2d> vecTestPoints;
	olc::vf2d vTestPointSize = { 6,6 };
	int nSelectedPoint = -1;

public:
	bool OnUserCreate() override
	{
		

		CreateImage(imLowRes, { 256, 240 });
		CreateImageFromFile(imHighResSprite, "e:/voxel.png");

		//CreateImage(imTest, { 64,64 });
		CreateImageFromFile(imLogo, "../tests/olc.png");
		CreateImageFromFile(imBlend, "../tests/blend.png");


		olc::ImageConfig cfg;
		cfg.MSAA = true;
		CreateImage(imTempBuffer, { 128, 128 }, cfg);

		CreateSampleTestImage(imSampleTest, { 32, 32 });

		CreateImage(imTemp, { 64, 64 });

		size_t x = 1;
		vecLogos.resize(x);
		for (auto& a : vecLogos)
		{
			a.pos = olc::vf2d(float(rand() % GetScreen().Size().x), float(rand() % GetScreen().Size().y));
			//a.vel = olc::vf2d(rand() % 100 - 50, rand() % 100 - 50);
			a.angvel = 1.1f;
		}

		vecVerts = {
			{ 100.0f,  100.0f},
			{ 200.0f, 100.0f},
			{ 200.0f, 200.0f},
			{ 100.0f,  200.0f}
		};

		
		vecTestPoints = {
			{ 16,  16},
			{ 64, 16},
			{ 16, 32},
			{  64,  32 },
			/*{16, 48},
			{64, 48},
			{ 16, 64},
			{64, 64 }*/
		};

		cube = CreateSanityCube();

		CreateImageFromFile(imSanityCube, "../../examples/assets/sanity_cube.png");

		return true;
	}

	struct mesh
	{
		std::vector<olc::vf4d> pos;
		std::vector<olc::vf4d> norm;
		std::vector<olc::vf2d> uv;
		std::vector<olc::Pixel> col;
		olc::Structure layout = olc::Structure::List;
	};

	olc::Image imSanityCube;


	inline mesh CreateSanityCube()
	{
		mesh m;


		/*			 5		   6
				1        2

					 4		   7
				0		 3

		*/

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

	mesh cube;
	olc::vf4d vCubePos = { 0,0,-5 };

	bool OnUserUpdate(float fElapsedTime) override
	{
		//draw.Line({ 0,0 }, mouse.GetPosition(), olc::Colour::WHITE);
		//return true;


		if (mouse.GetButton(1).bHeld)
		{
			fAngle += 0.5f * fElapsedTime;
		}

		

		// SW Rastering test
		draw.SetTarget(imTempBuffer);
		draw.Clear(olc::Colour::TANGERINE);
		draw.WorldRotate(fAngle, { 64.0f, 64.0f });// imLowRes.Size() / 2.0f);

		draw.Rect({ 0,0 }, draw.GetTargetSize()-1, olc::Colour::BLACK);


		/*draw.FilledTriangle(vecTestPoints[0] - olc::vi2d{64, 48}, vecTestPoints[1] - olc::vi2d{ 64, 48 }, vecTestPoints[2] - olc::vi2d{ 64, 48 },
			olc::Colour::RED,
			olc::Colour::GREEN,
			olc::Colour::BLUE);

		draw.FilledTriangle(vecTestPoints[0] - olc::vi2d{ 64, 48 }, vecTestPoints[2] - olc::vi2d{ 64, 48 }, vecTestPoints[3] - olc::vi2d{ 64, 48 },
			olc::Colour::RED,
			olc::Colour::BLUE,
			olc::Colour::YELLOW);*/


		std::vector<olc::vf2d> vecOffsetPoints(vecTestPoints.size());
		std::transform(
			vecTestPoints.begin(), vecTestPoints.end(),
			vecOffsetPoints.begin(),
			[](const olc::vf2d& v) { return v - olc::vf2d{ 64, 48 }; });

		std::vector<olc::Pixel> vecColours = {
			olc::Colour::RED,
			olc::Colour::GREEN,
			olc::Colour::BLUE,
			olc::Colour::DARK_YELLOW,
			olc::Colour::DARK_MAGENTA,
			olc::Colour::DARK_CYAN,
			olc::Colour::BLACK,
			olc::Colour::WHITE
		};


		
		

		//draw.Triangle(vecTestPoints[0] - olc::vi2d{ 64, 48 }, vecTestPoints[1] - olc::vi2d{ 64, 48 }, vecTestPoints[2] - olc::vi2d{ 64, 48 },
		//	olc::Colour::BLACK);

		//draw.Triangle(vecTestPoints[0] - olc::vi2d{ 64, 48 }, vecTestPoints[2] - olc::vi2d{ 64, 48 }, vecTestPoints[3] - olc::vi2d{ 64, 48 },
		//	olc::Colour::BLACK);

		draw.FilledRoundedRect(olc::vf2d{ 10.0f, 10.0f }, olc::vf2d{ 108.0f, 108.0f }, 10.0f, olc::Colour::BLUE, olc::Colour::WHITE, 8);
		draw.RoundedRect(olc::vf2d{ 10.0f, 10.0f }, olc::vf2d{ 108.0f, 108.0f }, 10.0f, olc::Colour::WHITE, olc::Colour::WHITE, 8);


		draw.FilledEllipse(olc::vf2d{ 64.0f, 64.0f }, 30, 20, olc::Colour::RED, olc::Pixel(255, 0, 0, 0), olc::Colour::WHITE, 16);
		draw.Ellipse(olc::vf2d{ 64.0f, 64.0f }, 30, 20, olc::Colour::BLACK, olc::Colour::WHITE, 16);


		draw.FilledCircle(olc::vf2d{ 64.0f, 64.0f }, 15, olc::Colour::GREEN, olc::Pixel(0, 255, 0, 0), olc::Colour::WHITE, 16);
		draw.Circle(olc::vf2d{ 64.0f, 64.0f }, 17, olc::Colour::BLACK, olc::Colour::WHITE, 16);


		olc::vf2d vScaledSize = olc::vf2d{ 8, 8 } / draw.GetWorldTransform().scale();
		draw.FilledRect(olc::vf2d{ 64.0f, 64.0f } - vScaledSize * 0.5, vScaledSize, olc::Pixel(255, 255, 0, 25));
		draw.Rect(olc::vf2d{ 64.0f, 64.0f } - vScaledSize * 0.5, vScaledSize, olc::Colour::BLACK);

		draw.FilledPolygon(
			olc::Structure::Strip,
			vecOffsetPoints,
			vecColours);

		draw.Polygon(
			olc::Structure::Strip,
			vecOffsetPoints,
			olc::Colour::BLACK
		);

	/*	draw.TexturedTriangle(
			vecTestPoints[0] - olc::vi2d{ 64, 48 }, vecTestPoints[1] - olc::vi2d{ 64, 48 }, vecTestPoints[2] - olc::vi2d{ 64, 48 },
			olc::Colour::RED,
			olc::Colour::GREEN,
			olc::Colour::BLUE,
			{ 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f },
			imLogo);*/


		draw.SetTarget(GetScreen());
		draw.Clear(olc::Colour::CYAN);

		/*draw.Triangle(vecTestPoints[0], vecTestPoints[1], vecTestPoints[2],
			olc::Colour::RED,
			olc::Colour::GREEN,
			olc::Colour::BLUE);

		draw.Triangle(vecTestPoints[0], vecTestPoints[2], vecTestPoints[3],
			olc::Colour::RED,
			olc::Colour::BLUE,
			olc::Colour::YELLOW);*/

		draw.Polygon(
			olc::Structure::Strip,
			vecTestPoints,
			vecColours);


		//draw.Image(imTempBuffer, { 64, 48 });
		draw.ImageQuad(imTempBuffer, vecTestPoints);
		//draw.Triangle(vecTestPoints[0], vecTestPoints[1], vecTestPoints[2], olc::Colour::BLACK);



		// Draw 3 triangle points
		for (int i = 0; i < vecTestPoints.size(); i++)
		{
			//draw.Rect(vecTestPoints[i] - (vTestPointSize * 0.5f), vTestPointSize, olc::Colour::MAGENTA);

			draw.FilledCircle(vecTestPoints[i],4, olc::Colour::GREEN, olc::Pixel(0,255,0,0), olc::Colour::WHITE, 16);
			draw.Circle(vecTestPoints[i], 4, olc::Colour::BLACK, olc::Colour::WHITE, 16);
		}



		// Testing matrices
		olc::mf4d t1, t2, t3;
		t1.translate(0.0f, 3.0f, 5.0f);
		t2.translate(0.0f, 6.0f, 0.0f);
		t3.translate(7.0f, 0.0f, 0.0f);

		olc::vf4d v1 = { 1,0,0,1 };
		olc::vf4d v2 = t3 * t2 * t1 * v1;


		olc::mf4d matProj;
		olc::mf4d matView;
		olc::mf4d matWorld;

		draw.SetViewport({ 0,0 }, GetScreen().Size());
		draw.MatrixReset();
		matProj.perspective(90.0f * 3.14159f / 180.0f, float(ScreenSize().x) / float(ScreenSize().y), 0.1f, 1000.0f);

		draw.SetProjectionMatrix(matProj);
		draw.SetViewMatrix(matView);

		if (keyboard.GetKey(olc::Key::LEFT).bHeld)
			vCubePos.x -= 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::RIGHT).bHeld)
			vCubePos.x += 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::UP).bHeld)
			vCubePos.y += 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::DOWN).bHeld)
			vCubePos.y -= 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::Q).bHeld)
			vCubePos.z += 5.0f * fElapsedTime;
		if (keyboard.GetKey(olc::Key::A).bHeld)
			vCubePos.z -= 5.0f * fElapsedTime;

		matWorld.translate(vCubePos);
		draw.SetModelMatrix(matWorld);

		for(int x = 0; x < 10; x++)
			for (int y = 0; y < 10; y++)
				for (int z = 0; z < 10; z++)
				{
					matWorld.translate(vCubePos + olc::vf4d(float(x) * 2.0f, float(y) * 2.0f, float(z) * 2.0f, 0));
					draw.SetModelMatrix(matWorld);
					//draw.SetMVPMatrix(matProj * matView * matWorld);
					draw.Mesh(cube.layout, cube.pos, cube.col, cube.uv, imSanityCube);
				}


		//draw3d.Mesh(cube.layout, cube.pos, cube.col);

		draw.Line({ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },olc::Colour::RED);
		draw.Line({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, olc::Colour::GREEN);
		draw.Line({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, olc::Colour::BLUE);

		

		// Handle mouse
		if (mouse.GetButton(0).bPressed)
		{
			nSelectedPoint = -1;
			for (int i = 0; i < vecTestPoints.size(); i++)
			{
				if ((mouse.GetPosition() - vecTestPoints[i]).mag2() < 9)
				{
					nSelectedPoint = i;
				}
			}
		}

		if (nSelectedPoint != -1 && mouse.GetButton(0).bHeld)
		{
			vecTestPoints[nSelectedPoint] = mouse.GetPosition().round();
		}

		if (mouse.GetButton(0).bReleased)
		{
			nSelectedPoint = -1;
		}

		draw.String({ 10, 10 }, mouse.GetPosition().str(), olc::Colour::BLACK);

		if (keyboard.GetKey(olc::Key::ESCAPE).bPressed)
			return false; // End application

		return true;


		//draw.SetTarget(imTemp);
		//draw.Clear(olc::Colour::BLANK);
		//draw.FilledRect({ 20,5 }, { 6, 54 }, olc::Colour::GREEN);
		//draw.FilledRect({ 0,0 }, imTemp.Size(), olc::Colour::BLANK);


		//draw.SetTarget(imgPrimary);
		//draw.Clear(olc::Colour::BLUE);


		//draw.WorldReset();
		//draw.FilledRect({ 0,0 }, imgPrimary.Size() * olc::vf2d(0.5f, 1.0f), olc::Pixel(0, 0, 0, 1));


	//	draw.SetTarget(imTempBuffer);
	//	draw.Clear(olc::Colour::WHITE);
	//	draw.Image(imBlend, { 0,0 });


	//	draw.SetTarget(GetDefaultImage());
	//	draw.WorldReset();
	//	draw.Clear(olc::Colour::TANGERINE);
	//	//draw.Image(imHighResSprite, mouse.GetPosition());


	//	//draw.SetTarget(imLowRes);

	//	//draw.Clear(olc::Colour::BLANK);

	//	if (mouse.GetButton(1).bHeld)
	//	{		
	//		fAngle += 0.2f * fElapsedTime;
	//	}

	//	draw.WorldRotate(fAngle, { 0,0 });// imLowRes.Size() / 2.0f);
	//	if (mouse.GetButton(2).bPressed)
	//	{
	//		auto w = std::make_shared<SecondWindow>();
	//		//AddChildWindow(w, { 100,100 }, { 2, 2 });
	//	}

	//	if (mouse.GetWheel() > 0)
	//	{
	//		fScale *= 1.1f;
	//	}

	//	if (mouse.GetWheel() < 0)
	//	{
	//		fScale *= 0.9f;
	//	}

	//	draw.WorldScale({ fScale, fScale });
	//	//for (int x = 0; x < imgPrimary.Size().x; x++)
	//	//	for (int y = 0; y < imgPrimary.Size().y; y++)
	//	//		draw.Pixel(olc::vf2d( x, y ), olc::Pixel(rand() % 255, rand() % 255, rand() % 255));

	// ////  
	//	//return true;




	//	//std::cout << mouse.GetPosition() << "\n";
	//	auto vMouse = draw.ScreenToWorld(mouse.GetPosition() );

	//	if (mouse.GetButton(0).bPressed)
	//	{
	//		float dist = 100000.0f;
	//		int idx = -1;
	//		for (int i = 0; i < 4; i++)
	//		{
	//			float d = (vecVerts[i] - vMouse).mag();
	//			if (d < 8 && d < dist)
	//			{
	//				dist = d;
	//				idx = i;
	//			}
	//		}
	//		nSelectedVert = idx;
	//	}

	//	

	//	if (nSelectedVert != -1 && mouse.GetButton(0).bHeld)
	//		vecVerts[nSelectedVert] = vMouse;

	//	if (mouse.GetButton(0).bReleased)
	//		nSelectedVert = -1;

	////	draw.ImageQuad(imLogo.region({ 0,0 }, { 10,10 }), vecVerts);

	////	draw.Line(vecVerts[0], vecVerts[1], olc::Colour::MAGENTA);
	////	draw.Line(vecVerts[1], vecVerts[2], olc::Colour::MAGENTA);
	////	draw.Line(vecVerts[2], vecVerts[3], olc::Colour::MAGENTA);
	////	draw.Line(vecVerts[3], vecVerts[0], olc::Colour::MAGENTA);

	////	//draw.Image(imTemp, vMouse, { 4,4 });

	////	

	//////draw.Image(olc::fontClassicPGE.imgFont, vMouse);

	////	

	////	std::string sTest = "Hello World!\nThe quick brown fox JUMPS over the LaZy dog...";
	////	olc::vf2d vSizeMono = draw.GetTextSize(sTest, false, { 1, 2 });
	////	olc::vf2d vSizeProp = draw.GetTextSize(sTest, true, { 1, 2 });


	////	draw.String({ 10.0f, 10.0f }, "Hello World!\nThe quick brown fox JUMPS over the LaZy dog...", olc::Colour::WHITE, { 1, 2 });
	////	draw.Rect({ 10.0f, 10.0f }, vSizeMono, olc::Colour::GREEN);

	////	draw.StringProp({ 10.0f, 100.0f }, "Hello World!\nThe quick brown fox JUMPS over the LaZy dog...", olc::Colour::BLUE, { 1, 2 });
	////	draw.Rect({ 10.0f, 100.0f }, vSizeProp, olc::Colour::GREEN);

	////	draw.StringProp({ 10.0f, 200.0f }, "Hello World!\nThe quick brown fox JUMPS over the LaZy dog...", olc::PixelF(0.5f, 0.0f, 0.0f, 0.25f), {1, 2});
	////	draw.Rect({ 10.0f, 200.0f }, vSizeProp, olc::Colour::GREEN);

	////	//draw.Pixel(vMouse, olc::Colour::GREEN);
	////	//draw.Line(vMouse, vMouse + 1,  olc::Colour::GREEN);
	////	
	////	draw.FilledRect({ 5, 100 }, { 100,100 }, olc::Colour::TANGERINE, olc::Colour::DARK_CYAN, olc::Colour::RED, olc::Colour::GREEN);
	////	
	////	draw.FilledRect({ 5,5 }, { 10,10 }, olc::Colour::YELLOW);



	//////	draw.Rect({ 8,8 }, { 20,20 }, olc::Colour::RED);

	////	if(mouse.GetButton(0).bHeld)
	////		draw.Line({ 1.0f, 1.0f }, { 25.5f, 25.5f });



	////	//draw.Image(imTempBuffer, vMouse);
	////	draw.Image(imSampleTest, vMouse);
	////	//draw.SetTarget(imgPrimary);
	////	//draw.WorldReset();
	////	//draw.Image(imLowRes, { 0,0 }, { 4, 4 });


	////	draw.swLine({ 100,100 }, vMouse, olc::Colour::BLACK, olc::Colour::YELLOW);


	////	

	////	draw.Rect({ 8,8, }, { 20,20 }, olc::Colour::RED, olc::Colour::GREEN, olc::Colour::BLUE, olc::Colour::WHITE);
	////	draw.swRect({ 7,7, }, { 22,22 }, olc::Colour::RED, olc::Colour::GREEN, olc::Colour::BLUE, olc::Colour::WHITE);

	//	for (int i = 0; i < 100; i++)
	//	{
	//		// Draw random triangle
	//		draw.swTexturedTriangle(
	//			{ float(rand() % GetDefaultImage().Size().x), float(rand() % GetDefaultImage().Size().y) },
	//			{ float(rand() % GetDefaultImage().Size().x), float(rand() % GetDefaultImage().Size().y) },
	//			{ float(rand() % GetDefaultImage().Size().x), float(rand() % GetDefaultImage().Size().y) },
	//			olc::Pixel(rand() % 256, rand() % 256, rand() % 256),
	//			olc::Pixel(rand() % 256, rand() % 256, rand() % 256),
	//			olc::Pixel(rand() % 256, rand() % 256, rand() % 256), { 0,0 }, { 0,1 }, { 1, 1 }, imLogo
	//		);
	//	}


	//	//std::cout << "Mouse Fil: " << vMouse << "\n";
	//	draw.swFilledTriangle({ 30,30 }, { 250,50 }, vMouse, olc::Colour::RED, olc::Colour::GREEN, olc::Colour::BLUE);
	//	//std::cout << "Mouse Fil: " << vMouse << "\n";
	//	draw.swFilledTriangle({ 30,30 }, { 250,50 }, vMouse, olc::Colour::WHITE);
	//	//std::cout << "Mouse Tex: " << vMouse << "\n";
	//	draw.swTexturedTriangle({ 30,30 }, { 250,50 }, vMouse, olc::Colour::RED, olc::Colour::GREEN, olc::Colour::BLUE, { 0,0 }, { 0,1 }, { 1, 1 }, imHighResSprite);

	//	draw.swTriangle({ 30,30 }, { 250,50 }, vMouse, olc::Colour::RED, olc::Colour::GREEN, olc::Colour::BLUE);
	//	

	//	//draw.Rect(vMouse, { 100,100 });

	//	/*for (auto& a : vecLogos)
	//	{
	//		a.pos += a.vel * fElapsedTime;
	//		a.ang += a.angvel * fElapsedTime;

	//		if (a.pos.x >= imgPrimary.Size().x - 46)
	//		{
	//			a.pos.x = imgPrimary.Size().x - 46;
	//			a.vel.x *= -1.0f;
	//		}

	//		if (a.pos.y >= imgPrimary.Size().y - 28)
	//		{
	//			a.pos.y = imgPrimary.Size().y - 28;
	//			a.vel.y *= -1.0f;
	//		}

	//		if (a.pos.x < 0)
	//		{
	//			a.pos.x = 0;
	//			a.vel.x *= -1.0f;
	//		}

	//		if (a.pos.y < 0)
	//		{
	//			a.pos.y = 0;
	//			a.vel.y *= -1.0f;
	//		}

	//		draw.ImageRotated(imLogo, a.pos, a.ang, { 10,10 }, { 2.0f, 4.0f });
	//	}*/
	//	


	//	/*draw.SetTarget(imTest);
	//	draw.FilledRect({ 0,0 }, imTest.Size(), olc::Colour::BLUE);

	//	draw.AffineRotate(fAngle, { 32.0f, 32.0f });
	//	
	//	draw.Rect({ 5,5 }, { 10,10 }, olc::Colour::YELLOW);
	//	draw.Line({ 0,0 }, { 20, 20 });


	//	draw.SetTarget(imgPrimary);
	//	draw.AffineReset();

	//	draw.FilledRect({ 0,0 }, imgPrimary.Size(), olc::Colour::VERY_DARK_MAGENTA);

	//	draw.Line({ 0,0 }, imgPrimary.Size() - 1, olc::Colour::RED);
	//	draw.Line(olc::vf2d( imgPrimary.Size().x - 1, 0 ), olc::vf2d( 0, imgPrimary.Size().y - 1 ), olc::Colour::GREEN);
	//	
	//	if(mouse.GetButton(0).bHeld)
	//		draw.Line({ 0,0 }, mouse.GetPosition(), olc::Colour::TANGERINE);

	//	draw.AffineRotate(fAngle * 0.2f, imgPrimary.Size() * 0.5f);
	//	draw.Rect({ 0,0 }, imgPrimary.Size());
	//	draw.Image(imTest, { 10, 10 }, { 64, 64 });
	//	draw.Image(imTest, { 100, 10 }, { 64, 64 });
	//	draw.Image(imTest, { 10, 100 }, { 64, 64 });

	//	draw.Image(sprite1, { 100, 100 }, { 64, 64 });
	//	draw.Image(sprite1.region({ 20.0f, 20.0f }, { 32.0f, 32.0f }), { 200, 100 }, { 32, 32 });		
	//	draw.Image(sprite1.region({ 20.0f, 20.0f }, { 32.0f, 32.0f }, { 100.0f, 200.0f }, { 150.0f, 180.0f }), { 100, 200 }, { 32, 32 });
	//	olc::Pixel p = draw.GetPixel(imTest, { 8,5 });

	//	draw.FilledRect({ 200,200 }, { 10,10 }, p);*/

	//	////draw.AffineRotate(fAngle, { 128,120 });
	//	////draw.AffineOffset({ 10,10 });
	//	//draw.AffineScale(olc::vf2d(1,1) * (std::sin(fAngle) + 1.1));

	//	//draw.Rect({ 0,0 }, GetSize() - olc::vf2d{1, 1}, olc::Colour::CYAN);
	//	//draw.FillRect({ 100.0f, 30.0f }, { 20.0f, 50.0f }, olc::Colour::TANGERINE);

	//	   

	//	//for (float i = 0; i < 20; i++)
	//	//{
	//	//	draw.AffineRotate(fAngle + (i/50.0f), {128,120});
	//	//	if (fmod(i,2) == 1)
	//	//	{
	//	//		draw.Line({ 0 + i * 4 ,0 + i * 4 }, { 255 - i * 4, 0 + i * 4 }, olc::Colour::RED, olc::Colour::GREEN);
	//	//		draw.Line({ 255 - i * 4,0 + i * 4 }, { 255 - i * 4, 239 - i * 4 }, olc::Colour::GREEN, olc::Colour::CYAN);
	//	//		draw.Line({ 255 - i * 4,239 - i * 4 }, { 0 + i * 4, 239 - i * 4 }, olc::Colour::CYAN, olc::Colour::WHITE);
	//	//		draw.Line({ 0 + i * 4,239 - i * 4 }, { 0 + i * 4, 0 + i * 4 }, olc::Colour::WHITE, olc::Colour::RED);
	//	//	}
	//	//	else
	//	//	{
	//	//		draw.Line({ 0 + i * 4 ,0 + i * 4 }, { 255 - i * 4, 0 + i * 4 }, olc::Colour::RED);
	//	//		draw.Line({ 255 - i * 4,0 + i * 4 }, { 255 - i * 4, 239 - i * 4 }, olc::Colour::GREEN);
	//	//		draw.Line({ 255 - i * 4,239 - i * 4 }, { 0 + i * 4, 239 - i * 4 }, olc::Colour::CYAN);
	//	//		draw.Line({ 0 + i * 4,239 - i * 4 }, { 0 + i * 4, 0 + i * 4 }, olc::Colour::WHITE);
	//	//	}
	//	//}

	//	///*for (int x = 0; x < 100; x++)
	//	//{
	//	//	for (int y = 0; y < 100; y++)
	//	//	{
	//	//		if (x % 2 == 0)
	//	//		{
	//	//			draw.Pixel(olc::vf2d(x, y) + olc::vf2d(100, 100), olc::Colour::YELLOW);
	//	//		}
	//	//	}
	//	//}*/

	//	////draw.Pixel({ -0.5f, -0.25f }, olc::Colour::YELLOW);

	//	//draw.FillRect({ 100.0f, 30.0f }, { 20.0f, 50.0f }, olc::Colour::TANGERINE);

	//	////draw.Pixel({ 0.3f, 0.3f }, olc::Colour::BLACK);
	//	////draw.Pixel({ 0.32f, 0.3f }, olc::Colour::BLACK);
	//	////draw.Pixel({ 0.34f, 0.3f }, olc::Colour::BLACK);




		return true;
	}
};

int main()
{
	Example demo;
	//if (demo.Construct({ 256, 240 }, { 4, 4 }))

	olc::PGEConfig cfg;
	cfg.vPixelSize = { 1,1 };
	cfg.vScreenSize = { 1024, 960 };

	//cfg.vPixelSize = { 4,4 };
	//cfg.vScreenSize = { 256, 240 };
	//cfg.bAntiAliasMainScreen = true;
	cfg.bVSync = false;

	//if (demo.Construct({ 1280, 960 }, { 1, 1 }, cfg))
	if(demo.Construct(cfg))
		demo.Start();

	return 0;
}
