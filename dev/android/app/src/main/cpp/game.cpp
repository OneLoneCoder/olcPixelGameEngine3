#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

#include <android/log.h>

class Example : public olc::PixelGameEngine
{
public:
    Example()
    {

    }

// protected:
//     // We accumulate total time for some animation
//     float fTotalTime = 0.0f;

//     olc::gpu::Shader_GLSL33 shaderExample;
//     olc::Image imgWithoutFX;

//     olc::Image imgGameScene;

//     olc::vf2d vShockwaveOrigin;
//     float fShockwaveTime = 1000.0f;

// public:
//     // Called once at the start, so create things here
//     bool OnUserCreate() override
//     {

//         // Custom pixel shader main() function and uniforms
//         std::string custom_ps_main =
//                 R"(
// 		uniform vec2 sw_origin;
// 		uniform float sw_radius;

// 		float wave(vec2 p)
// 		{
// 			float waveAmplitude = 1.;
// 			float waveThickness = 0.075;
// 			float waveRadius = sw_radius * 0.5;

// 			float distanceFromOrigin = length(p);
// 			float distanceFromWave = abs(waveRadius - distanceFromOrigin);

// 			if (distanceFromWave <= waveThickness)
// 			{
// 				//             wave                    gate                          height
// 				return cos(distanceFromWave) * (waveThickness - distanceFromWave) * waveAmplitude;
// 			}
// 			else
// 			{
// 				return 0.;
// 			}
// 		}

// 		void main()
// 		{
// 			// Expanding shockwave centered on origin effect

// 			// get pixel position relative to shockwave origin
// 			vec2 p = oTex - sw_origin;

// 			// offset sample position radially by wave "height"
// 			float h = wave(p);
// 			vec2 samplePos = oTex + normalize(p) * h;

// 			// Sample underlying texture
// 			vec4 texColour = texture(pgeTexture0, samplePos) * oCol;

// 			// knock out some red to give a blueish shockwave tint
// 			texColour.r *= (1.0 - h * 10.0);
// 			pixel = vec4(texColour.rgb * texColour.a, texColour.a);
// 		}
// 		)";


//         // Construct pixel shader from default header, but use custom main()
//         shaderExample.SetPixelShaderSource(
//                 olc::gpu::Shader::PS_DefaultHeader() +
//                 custom_ps_main);

//         // Just use the PGE3 default vertex shader
//         shaderExample.SetVertexShaderSource(
//                 olc::gpu::Shader::VS_DefaultHeader() +
//                 olc::gpu::Shader::VS_DefaultMain()
//         );

//         // Check for shader compile errors
//         std::string sResult = shaderExample.Compile();
//         if (sResult != "OK")
//         {
//             std::cout << "Error compiling shader: " << sResult << std::endl;
//             return false;
//         }

//         // Add custom uniforms used by custom shader. The "system"
//         // will have taken care of required PGE3 uniforms
//         shaderExample.CreateUniform("sw_origin");
//         shaderExample.CreateUniform("sw_radius");

//         // Create off-screen image to draw to
//         CreateImage(imgWithoutFX, GetDefaultImage().Size());

//         // Load a fake game scene to demonstrate effect on
//         CreateImageFromFile(imgGameScene, "gamescene.png");
//         return true;
//     }

//     // Called every frame, so update things here
//     bool OnUserUpdate(float fElapsedTime) override
//     {
//         // DO SIMULATED GAME LOGIC HERE

//         // For this demo, shockwave always exists and expands
//         fShockwaveTime += fElapsedTime * 2.0f;
//         if (mouse.GetButton(0).bPressed)
//         {
//             vShockwaveOrigin = mouse.GetPosition();
//             fShockwaveTime = 0.0f;
//         }

//         // NOW DRAW SCENE

//         // Set rendering target to off-screen image, we will draw to this
//         // entirely normally, then apply a custom shader when we draw the
//         // off-screen image to the screen
//         draw.SetTarget(imgWithoutFX);

//         // DO ALL GAME DRAWING HERE

//         // Draw a simulated fake game screen
//         draw.Image(imgGameScene, { 0, 0 });



//         // Copy image to screen with new shader
//         draw.SetTarget(GetDefaultImage());

//         // Set the custom shader
//         draw.SetShader(shaderExample);
//         draw.SetShaderUniform("sw_origin", vShockwaveOrigin / olc::vf2d(imgGameScene.Size()));
//         draw.SetShaderUniform("sw_radius", fShockwaveTime);

//         // Present image with shader effect - this is required
//         // because here is where the shader is actually applied
//         draw.Image(imgWithoutFX, { 0, 0 });

//         // Successful frame
//         return true;
//     }
protected:
	
	olc::vf2d vPosition;
	std::string sText;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		vPosition = ScreenSize() / 2;
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Move position with arrow keys
		if (keyboard.GetKey(olc::Key::UP).bHeld)	
			vPosition.y -= 50.0f * fElapsedTime;

		if (keyboard.GetKey(olc::Key::DOWN).bHeld)
			vPosition.y += 50.0f * fElapsedTime;

		if (keyboard.GetKey(olc::Key::LEFT).bHeld)
			vPosition.x -= 50.0f * fElapsedTime;

		if (keyboard.GetKey(olc::Key::RIGHT).bHeld)
			vPosition.x += 50.0f * fElapsedTime;

		draw.FilledCircle(vPosition.round(), 10.0f);


		// Capture text input from keyboard
		// Note: this is specifically for text entry and shouldnt
		// be used for key polling.
		for (const auto& key : keyboard.GetKeyCache())
		{
			// Each olc::Key has a glyph associated with it
			// depending on the current keyboard layout, which
			// can be queried via GetKeyGlyph()
			if (key == olc::Key::BACK && !sText.empty())
				sText.pop_back();
			else
				sText += keyboard.GetKeyGlyph(key, keyboard.GetKey(olc::Key::SHIFT).bHeld);
		}

		// Display current keyboard layout
		std::string sKeyboardType;
		auto kbl = pHost->GetKeyboardLayout();
		if(kbl == olc::KeyboardLayout::QWERTY_UK)
			sKeyboardType = "QWERTY UK";
		else if (kbl == olc::KeyboardLayout::QWERTY_US)
			sKeyboardType = "QWERTY US";
		else if (kbl == olc::KeyboardLayout::AZERTY)
			sKeyboardType = "AZERTY";
		else if (kbl == olc::KeyboardLayout::QWERTZ)
			sKeyboardType = "QWERTZ";
		else
			sKeyboardType = "UNKNOWN";

		draw.StringProp({ 10, 5 }, "Keyboard Layout: " + sKeyboardType, olc::Colour::YELLOW);
		draw.StringProp({ 10,20 }, sText, olc::Colour::GREEN);
		

		// Successful frame
		return true;
	}
};

int main(int argc, char** argv) {
    __android_log_print(ANDROID_LOG_INFO, "PGE3 Game", "Started native_main");

    // Construct demo application
    Example demo;

    // Create "screen" of 256x240 "pixels"
    // with a pixel size of 4x4 actual screen pixels
    if (demo.Construct({ 256, 240 }, { 4, 4 })) {
        demo.Start();
    }

    return 0;
}
