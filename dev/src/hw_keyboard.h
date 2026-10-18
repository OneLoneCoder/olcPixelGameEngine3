#pragma once

//! START STDHEADER
#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <string>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "hw_input.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_HW_KEYBOARD_DECLARED)
namespace olc
{
	// Forward declare for friendship
	class Window;
	class PGEWindow;

	enum class KeyboardLayout : uint8_t
	{
		QWERTY_UK,
		QWERTY_US,
		QWERTZ,
		AZERTY
	};

	// Officially recognised OLC key codes
	enum class Key : uint8_t
	{
		NONE,
		
		// Alphanumeric keys
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		
		// Numeric keys
		K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
		
		// Numpad keys
		NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
		NP_MUL, NP_DIV, NP_ADD, NP_SUB, NP_DECIMAL, PERIOD,
		
		// Function keys
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		
		// Arrow keys
		UP, DOWN, LEFT, RIGHT,
		
		// Other keys
		SPACE, TAB, SHIFT, CTRL, ALT, INS, DEL, HOME, END, PGUP, PGDN, CAPS_LOCK, 
		BACK, ESCAPE, RETURN, ENTER, PAUSE, SCROLL,	EQUALS, COMMA, MINUS,
		
		// OEM specific keys
		OEM_1, OEM_2, OEM_3, OEM_4, OEM_5, OEM_6, OEM_7, OEM_8,

		// Special case for iteration limits
		ENUM_END
	};


	namespace hw
	{
		class Keyboard
		{
			friend class olc::Window;
			friend class olc::PGEWindow;

		public:
			Keyboard() = default;

		public:
			// Get the state of a specific key
			const Button& GetKey(const olc::Key nOLCKeyCode) const;
			// Get a list of keys that have been pressed/typed since last update
			const std::vector<olc::Key>& GetKeyCache() const;
			// Get printable glyph for a key based on current modifier state
			const std::string GetKeyGlyph(const olc::Key key, const bool shift = false, const bool ctrl = false, const bool alt = false) const;
			// Converts System keycode to olc::Key
			olc::Key SystemKeyCodeToOLCKey(const int32_t nSystemKeyCode) const;
			// Set keyboard layout for glyph mapping
			void UseKeyboardLayout(const olc::KeyboardLayout kbl);

		public: // Text Entry Interface

			// Text is handled differently to keyboard input, but is fundamentally
			// driven by keyboard input (or at least events that smell like a keyboard)
			//
			// To use, EnableTextEntry() which then starts to accumulate keyboard 
			// input from OS. A cursor is also maintained that can be moved with
			// left and right arrow keys, and delete and backspace work as expected.
			//
			// When the user presses a delimiter key (e.g. enter), the current text
			// input is added to a command buffer the user can interrogate. This
			// method is used because some 3rd party device drivers may leverage
			// keyboard input to do other interesting things like barcode readers.
			//
			// Being in Text entry mode does not prohibit normal keyboard behaviour
			// so endure your application handles keyboard input appropriately.
			//
			// Leaving TextEntryMode happens automatically when delimiter encountered

			// Enter Text Entry Mode
			void EnableTextEntry(const std::string_view sDefault = "", const std::string_view delim = "\n", const bool bLeaveMode = true);
			// Leave Text Entry Mode
			void DisableTextEntry();
			// Is TextEntryMode enabled?
			bool TextEntryEnabled() const;
			// Get the current accumulated text input
			const std::string& GetCurrentText() const;
			// Get the current cursor position within the input string
			int32_t GetTextCursorPosition() const;
			// Get a RO vector of current commands
			const std::vector<std::string>& GetTextCommands() const;
			// User must clear accumulated commands when appropriate
			void ClearTextCommands();

		private:
			// Set key state from system keycode
			void SetKey(const olc::Key key, bool state);
			// Update internal state, called once per frame
			void UpdateState();

		protected:
			std::array<Button, 256> keys{};
			std::array<bool, 256> keys_new{};
			std::array<bool, 256> keys_old{};

		private:
			// We use two buffers to cache key presses so we can 
			// read from one while writing to the other, we dont
			// want to miss any key presses that are typed, or 
			// autotyped by the OS repeat rate or peripheral
			std::array<std::vector<olc::Key>, 2> keyCache{};
			size_t nKeyCacheIndex = 0;

			bool bTextEntryMode = false;
			std::string sTextEntryString = "";
			int32_t nTextEntryCursorPos = 0;
			std::vector<std::string> vecTextEntryCommands;
			std::string sTextEntryDelim = "\n";
			bool bLeaveTextEntryModeUponDelimiter = true;

			// Map of olc::Keycodes to printable glyphs
			struct sKeyGlyph
			{
				std::string modNone;
				std::string modShift;
				std::string modCtrl;
				std::string modAlt;
			};
			std::unordered_map<olc::Key, sKeyGlyph> mapKeyGlyphs;


		};
	}
}
#define PGE_HW_KEYBOARD_DECLARED 1
#endif
