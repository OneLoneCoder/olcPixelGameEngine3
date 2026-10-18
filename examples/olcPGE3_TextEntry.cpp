/*
	olc::PixelGameEngine3 Example - Text Entry Mode

	Allows the user to enter and edit text

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating text entry and editing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_TextEntryMode : public olc::PixelGameEngine
{
public:
	Example_TextEntryMode()
	{
		sAppName = "Example - Text Entry Mode";
	}

protected:
	float fTickerTime = 0.0f;

	// We'll draw random text in the background with
	// a typing effect
	std::list<std::string> listHistory;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		listHistory.push_back("Welcome to Text Entry Demo!");
		listHistory.push_back("");
		listHistory.push_back("Press 'TAB' to enter text entry mode");
		listHistory.push_back("Type your message");
		listHistory.push_back("Move cursor with LEFT & RIGHT");
		listHistory.push_back("Edit message with backspace and delete ");
		listHistory.push_back("");
		listHistory.push_back("Pressing 'ENTER' will force a 'command'");
		listHistory.push_back("Give it a go!");
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Check if commands from text entry mode exist
		if (!keyboard.GetTextCommands().empty())
		{
			// There are commands waiting to be processed
			for (const auto& sNewCommand : keyboard.GetTextCommands())
			{
				// capture them by adding to history queue
				listHistory.push_back(sNewCommand);
				if (listHistory.size() > 10)
					listHistory.pop_front();
			}

			// Up to user to remove commands when appropriate
			keyboard.ClearTextCommands();
		}


		// Draw Command History
		olc::vf2d vPos = { 10,10 };
		for (const auto& sCommand : listHistory)
		{
			draw.StringProp(vPos, sCommand, olc::Colour::CYAN);
			vPos.y += 12;
		}


		if (keyboard.GetKey(olc::Key::TAB).bPressed)
		{
			if (keyboard.TextEntryEnabled())
			{
				keyboard.DisableTextEntry();
			}
			else
			{
				// Start text entry. In this instance we start with a 
				// blank input string, and we want to respond to the 
				// enter key being pressed. The false parameter tells 
				// the system not to leave text entry mode when a 
				// command has been entered, whcih for this demo is
				// a lot more fluid.
				keyboard.EnableTextEntry("", "\n", false);
			}
		}

		if (keyboard.TextEntryEnabled())
		{
			// Read current text, and virtual cursor pos
			const std::string& sText = keyboard.GetCurrentText();
			const uint32_t nCursor = keyboard.GetTextCursorPosition();

			// Work out where the cursor should be drawn
			const auto vTextSizeToCursor = draw.GetTextSize(sText.substr(0, nCursor), true);

			olc::vf2d vOffset = { 10,140 };

			// Draw Cursor
			draw.FilledRect(vOffset + olc::vf2d(0, -10) + vTextSizeToCursor, { 2, 10 }, olc::Colour::YELLOW);

			// Draw Entered Text
			draw.StringProp(vOffset, sText);

		}

		
		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_TextEntryMode demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}