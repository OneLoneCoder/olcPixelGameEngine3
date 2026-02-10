/*
	olc::PixelGameEngine3 Example - Mouse

	Demonstrates using the mouse input system

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#include <string>
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"
#include <numbers>

struct Particle
{
    olc::vf2d pos;
    olc::vf2d vel;
    olc::Pixel color;
    float life;
    float maxLife;
};

// Example application demonstrating mouse input. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Mouse : public olc::PixelGameEngine
{
public:
	Example_Mouse()
	{
		
	}

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::DARK_BLUE);
		
		// Get the rounded version of the mouse position
		olc::vf2d mousePos = mouse.GetPosition().round();

        // Update scroll accumulator
		scrollAcculator += mouse.GetWheel()  * fElapsedTime;
		scrollAcculator *= 0.95f; // Friction/Decay
		
		olc::vi2d barSize{200, 20};
		olc::vi2d barPosition = olc::vi2d{(ScreenSize().x / 2) - (barSize.x / 2), ScreenSize().y - (barSize.y + 10)};
		DrawScrollIndicator(barPosition, barSize, scrollAcculator);

		// Create particles on mouse buttons
        if(mouse.GetButton(0).bPressed) // Left click
            SpawnParticles(mousePos, olc::Colour::RED, 20);
        
		if(mouse.GetButton(1).bPressed) // Right click
            SpawnParticles(mousePos, olc::Colour::BLUE, 20);
        
		if(mouse.GetButton(2).bPressed) // Middle click
            SpawnParticles(mousePos, olc::Colour::GREEN, 20);

		if(mouse.GetButton(3).bPressed)
            SpawnParticles(mousePos, olc::Colour::MAGENTA, 20);

		if(mouse.GetButton(4).bPressed)
            SpawnParticles(mousePos, olc::Colour::TANGERINE, 20);

		// Update and Draw Particles
		for(auto &p : vecParticles)
		{
			p.life -= fElapsedTime;
			p.pos += p.vel * fElapsedTime;
			p.vel *= 0.98f;  // Friction
			p.color.a = (uint8_t)((p.life / p.maxLife) * 255);
			
			draw.FilledCircle(p.pos, 3, p.color);
		}

		// Remove dead particles
		vecParticles.erase(
			std::remove_if(vecParticles.begin(), vecParticles.end(),
			[](Particle p) -> bool { return p.life <= 0.0f; }),
			vecParticles.end()
		);
        
		// Draw cursor
        draw.Circle(mousePos, 8, olc::Colour::WHITE);
        draw.Circle(mousePos, 4, olc::Colour::CYAN);
        
		if(mouse.GetButton(0).bHeld)
            draw.Circle(mousePos, 14, olc::Colour::RED);
        
		if(mouse.GetButton(1).bHeld)
            draw.Circle(mousePos, 18, olc::Colour::BLUE);
        
		if(mouse.GetButton(2).bHeld)
            draw.Circle(mousePos, 22, olc::Colour::GREEN);

		if(mouse.GetButton(3).bHeld)
            draw.Circle(mousePos, 26, olc::Colour::MAGENTA);
        
		if(mouse.GetButton(4).bHeld)
            draw.Circle(mousePos, 28, olc::Colour::TANGERINE);
		
		// Instructions
		draw.String({10, 10}, "Mouse Example\n\nClick all the buttons!\nScroll the wheel!", olc::Colour::YELLOW);
		
		// Successful frame
		return true;
	}
	
	void DrawScrollIndicator(olc::vf2d pos, olc::vf2d size, float& indicatorPosition)
	{
		indicatorPosition = std::clamp(indicatorPosition, -1.0f, 1.0f);
		draw.FilledRect(pos, size, olc::Colour::DARK_GREY);
		draw.Rect(pos, size, olc::Colour::WHITE);
		
		olc::vf2d halfTextSize = draw.GetTextSize("SCROLL WHEEL") / 2;
		draw.String(pos + (size / 2) - halfTextSize, "SCROLL WHEEL");
		
		draw.FilledRect(
			{pos.x + (size.x / 2) + (indicatorPosition * (size.x * .5f)) - 2, pos.y},
			{4, size.y},
			olc::Colour::YELLOW
		);
	}

	void SpawnParticles(olc::vf2d pos, olc::Pixel color, int count)
	{
        for(int i = 0; i < count; i++)
		{
			float angle = (rand() / (float)RAND_MAX) * 2.0f * std::numbers::pi;
            float speed = 50.0f + (rand() / (float)RAND_MAX) * 100.0f;
            
            Particle p;
            p.pos = pos;
            p.vel = olc::vf2d{
				std::cos(angle) * speed,
				std::sin(angle) * speed
			};
            p.color = color;
            p.life = 1.0f + (rand() / (float)RAND_MAX) * 1.0f;
            p.maxLife = p.life;
            
            vecParticles.push_back(p);
        }
	}
private:
	std::vector<Particle> vecParticles;
	float scrollAcculator = 0.0f;
};

// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Mouse demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	PGEConfig config;
	config.bVSync = true;
	config.vPixelSize = { 4,4 };
	config.vScreenSize = { 256,240 };

	if (demo.Construct(config))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}