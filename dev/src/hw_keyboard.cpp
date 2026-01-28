#include "hw_keyboard.h"

#if OLC_HOST == OLC_HOST_WINDOWS
#include "host_win_winapi.h"
#include <WinUser.h>
#endif


//! START IMPLEMENTATION
namespace olc::hw
{

    void Keyboard::UseKeyboardLayout(const olc::KeyboardLayout kbl)
    {
        mapKeyGlyphs.clear();

		//  Define appropriate glyphs for each key for the selected keyboard layout
        if (kbl == olc::KeyboardLayout::QWERTY_UK)
        {
            mapKeyGlyphs =
            {
                // PGE Key, no mods, shift mod, ctrl mod, alt mod
                {olc::Key::A, {"a", "A", "a", "a"}},
                {olc::Key::B, {"b", "B", "b", "b"}},
                {olc::Key::C, {"c", "C", "c", "c"}},
                {olc::Key::D, {"d", "D", "d", "d"}},
                {olc::Key::E, {"e", "E", "e", "e"}},
                {olc::Key::F, {"f", "F", "f", "f"}},
                {olc::Key::G, {"g", "G", "g", "g"}},
                {olc::Key::H, {"h", "H", "h", "h"}},
                {olc::Key::I, {"i", "I", "i", "i"}},
                {olc::Key::J, {"j", "J", "j", "j"}},
                {olc::Key::K, {"k", "K", "k", "k"}},
                {olc::Key::L, {"l", "L", "l", "l"}},
                {olc::Key::M, {"m", "M", "m", "m"}},
                {olc::Key::N, {"n", "N", "n", "n"}},
                {olc::Key::O, {"o", "O", "o", "o"}},
                {olc::Key::P, {"p", "P", "p", "p"}},
                {olc::Key::Q, {"q", "Q", "q", "q"}},
                {olc::Key::R, {"r", "R", "r", "r"}},
                {olc::Key::S, {"s", "S", "s", "s"}},
                {olc::Key::T, {"t", "T", "t", "t"}},
                {olc::Key::U, {"u", "U", "u", "u"}},
                {olc::Key::V, {"v", "V", "v", "v"}},
                {olc::Key::W, {"w", "W", "w", "w"}},
                {olc::Key::X, {"x", "X", "x", "x"}},
                {olc::Key::Y, {"y", "Y", "y", "y"}},
                {olc::Key::Z, {"z", "Z", "z", "z"}},

                {olc::Key::K0, {"0", ")", "0", "0"}},
                {olc::Key::K1, {"1", "!", "1", "1"}},
                {olc::Key::K2, {"2", "\"","2", "2"}},
                {olc::Key::K3, {"3", "#", "3", "3"}},
                {olc::Key::K4, {"4", "$", "4", "4"}},
                {olc::Key::K5, {"5", "%", "5", "5"}},
                {olc::Key::K6, {"6", "^", "6", "6"}},
                {olc::Key::K7, {"7", "&", "7", "7"}},
                {olc::Key::K8, {"8", "*", "8", "8"}},
                {olc::Key::K9, {"9", "(", "9", "9"}},

                {olc::Key::NP0, {"0", "0", "0", "0"}},
                {olc::Key::NP1, {"1", "1", "1", "1"}},
                {olc::Key::NP2, {"2", "2", "2", "2"}},
                {olc::Key::NP3, {"3", "3", "3", "3"}},
                {olc::Key::NP4, {"4", "4", "4", "4"}},
                {olc::Key::NP5, {"5", "5", "5", "5"}},
                {olc::Key::NP6, {"6", "6", "6", "6"}},
                {olc::Key::NP7, {"7", "7", "7", "7"}},
                {olc::Key::NP8, {"8", "8", "8", "8"}},
                {olc::Key::NP9, {"9", "9", "9", "9"}},
                {olc::Key::NP_MUL, {"*", "*", "*", "*"}},
                {olc::Key::NP_DIV, {"/", "/", "/", "/"}},
                {olc::Key::NP_ADD, {"+", "+", "+", "+"}},
                {olc::Key::NP_SUB, {"-", "-", "-", "-"}},
                {olc::Key::NP_DECIMAL, {".", ".", ".", "."}},

                {olc::Key::PERIOD, {".", ">", ".", "."}},
                {olc::Key::EQUALS, {"=", "+", "=", "="}},
                {olc::Key::COMMA, {",", "<", ",", ","}},
                {olc::Key::MINUS, {"-", "_", "-", "-"}},
                {olc::Key::SPACE, {" ", " ", " ", " "}},
                {olc::Key::ENTER, {"\n", "\n ", "\n", "\n"}},

                {olc::Key::OEM_1, {";", ":", ";", ";"}},
                {olc::Key::OEM_2, {"/", "?", "/", "/"}},
                {olc::Key::OEM_3, {"\'","@", "\'", "\'"}},
                {olc::Key::OEM_4, {"[", "{", "[", "["}},
                {olc::Key::OEM_5, {"\\", "|", "\\", "\\"}},
                {olc::Key::OEM_6, {"]", "}", "]", "]"}},
                {olc::Key::OEM_7, {"#", "~", "#", "#"}},

                // Give these keys glyphs so they can be interpreted in text editing
                {olc::Key::TAB, {"\t", "\t", "\t", "\t"}},
                {olc::Key::BACK, {"\b", "\b", "\b", "\b"}},
                {olc::Key::DEL, {"_X", "_X", "_X", "_X"}},
                {olc::Key::LEFT, {"_L", "_L", "_L", "_L"}},
                {olc::Key::RIGHT, {"_R", "_R", "_R", "_R"}},
                {olc::Key::UP, {"_U", "_U", "_U", "_U"}},
                {olc::Key::DOWN, {"_D", "_D", "_D", "_D"}},
            };
        }

        //  Define appropriate glyphs for each key for the selected keyboard layout
        if (kbl == olc::KeyboardLayout::QWERTY_US)
        {
            mapKeyGlyphs =
            {
                // PGE Key, no mods, shift mod, ctrl mod, alt mod
                {olc::Key::A, {"a", "A", "a", "a"}},
                {olc::Key::B, {"b", "B", "b", "b"}},
                {olc::Key::C, {"c", "C", "c", "c"}},
                {olc::Key::D, {"d", "D", "d", "d"}},
                {olc::Key::E, {"e", "E", "e", "e"}},
                {olc::Key::F, {"f", "F", "f", "f"}},
                {olc::Key::G, {"g", "G", "g", "g"}},
                {olc::Key::H, {"h", "H", "h", "h"}},
                {olc::Key::I, {"i", "I", "i", "i"}},
                {olc::Key::J, {"j", "J", "j", "j"}},
                {olc::Key::K, {"k", "K", "k", "k"}},
                {olc::Key::L, {"l", "L", "l", "l"}},
                {olc::Key::M, {"m", "M", "m", "m"}},
                {olc::Key::N, {"n", "N", "n", "n"}},
                {olc::Key::O, {"o", "O", "o", "o"}},
                {olc::Key::P, {"p", "P", "p", "p"}},
                {olc::Key::Q, {"q", "Q", "q", "q"}},
                {olc::Key::R, {"r", "R", "r", "r"}},
                {olc::Key::S, {"s", "S", "s", "s"}},
                {olc::Key::T, {"t", "T", "t", "t"}},
                {olc::Key::U, {"u", "U", "u", "u"}},
                {olc::Key::V, {"v", "V", "v", "v"}},
                {olc::Key::W, {"w", "W", "w", "w"}},
                {olc::Key::X, {"x", "X", "x", "x"}},
                {olc::Key::Y, {"y", "Y", "y", "y"}},
                {olc::Key::Z, {"z", "Z", "z", "z"}},

                {olc::Key::K0, {"0", ")", "0", "0"}},
                {olc::Key::K1, {"1", "!", "1", "1"}},
                {olc::Key::K2, {"2", "@","2", "2"}},
                {olc::Key::K3, {"3", "#", "3", "3"}},
                {olc::Key::K4, {"4", "$", "4", "4"}},
                {olc::Key::K5, {"5", "%", "5", "5"}},
                {olc::Key::K6, {"6", "^", "6", "6"}},
                {olc::Key::K7, {"7", "&", "7", "7"}},
                {olc::Key::K8, {"8", "*", "8", "8"}},
                {olc::Key::K9, {"9", "(", "9", "9"}},

                {olc::Key::NP0, {"0", "0", "0", "0"}},
                {olc::Key::NP1, {"1", "1", "1", "1"}},
                {olc::Key::NP2, {"2", "2", "2", "2"}},
                {olc::Key::NP3, {"3", "3", "3", "3"}},
                {olc::Key::NP4, {"4", "4", "4", "4"}},
                {olc::Key::NP5, {"5", "5", "5", "5"}},
                {olc::Key::NP6, {"6", "6", "6", "6"}},
                {olc::Key::NP7, {"7", "7", "7", "7"}},
                {olc::Key::NP8, {"8", "8", "8", "8"}},
                {olc::Key::NP9, {"9", "9", "9", "9"}},
                {olc::Key::NP_MUL, {"*", "*", "*", "*"}},
                {olc::Key::NP_DIV, {"/", "/", "/", "/"}},
                {olc::Key::NP_ADD, {"+", "+", "+", "+"}},
                {olc::Key::NP_SUB, {"-", "-", "-", "-"}},
                {olc::Key::NP_DECIMAL, {".", ".", ".", "."}},

                {olc::Key::PERIOD, {".", ">", ".", "."}},
                {olc::Key::EQUALS, {"=", "+", "=", "="}},
                {olc::Key::COMMA, {",", "<", ",", ","}},
                {olc::Key::MINUS, {"-", "_", "-", "-"}},
                {olc::Key::SPACE, {" ", " ", " ", " "}},
                {olc::Key::ENTER, {"\n", "\n ", "\n", "\n"}},

                {olc::Key::OEM_1, {";", ":", ";", ";"}},
                {olc::Key::OEM_2, {"/", "?", "/", "/"}},
                {olc::Key::OEM_3, {"`","~", "`", "`"}},
                {olc::Key::OEM_4, {"[", "{", "[", "["}},
                {olc::Key::OEM_5, {"\\", "|", "\\", "\\"}},
                {olc::Key::OEM_6, {"]", "}", "]", "]"}},
                {olc::Key::OEM_7, {"'", "\"", "'", "'"}},

                // Give these keys glyphs so they can be interpreted in text editing
                {olc::Key::TAB, {"\t", "\t", "\t", "\t"}},
                {olc::Key::BACK, {"\b", "\b", "\b", "\b"}},
                {olc::Key::DEL, {"_X", "_X", "_X", "_X"}},
                {olc::Key::LEFT, {"_L", "_L", "_L", "_L"}},
                {olc::Key::RIGHT, {"_R", "_R", "_R", "_R"}},
                {olc::Key::UP, {"_U", "_U", "_U", "_U"}},
                {olc::Key::DOWN, {"_D", "_D", "_D", "_D"}},
            };
        }
    }

    const Button& Keyboard::GetKey(const olc::Key key) const
    {
		return keys.at(size_t(key));
    }

    const std::vector<olc::Key>& Keyboard::GetKeyCache() const
    {
		return keyCache[nKeyCacheIndex ^ 0x01]; // Return the inactive cache
    }

    const std::string Keyboard::GetKeyGlyph(const olc::Key key, const bool shift, const bool ctrl, const bool alt) const
    {
        if (mapKeyGlyphs.contains(key))
        {
            if(shift)
                return mapKeyGlyphs.at(key).modShift;
            else if(ctrl)
                return mapKeyGlyphs.at(key).modCtrl;
            else if(alt)
                return mapKeyGlyphs.at(key).modAlt;
            else
				return mapKeyGlyphs.at(key).modNone;
        }

        return "";
    }

    void Keyboard::SetKey(const olc::Key key, bool state)
    {
        keys_new[size_t(key)] = state;

        // Update key cache
        if (state)
        {
            keyCache[nKeyCacheIndex].push_back(key);
        }
	}

   
    void olc::hw::Keyboard::UpdateState()
    {
        for (size_t i = 0; i < keys.size(); i++)
        {
            keys[i].bPressed = false;
            keys[i].bReleased = false;
            if (keys_new[i] != keys_old[i])
            {
                if (keys_new[i])
                {
                    keys[i].bPressed = !keys[i].bHeld;
                    keys[i].bHeld = true;
                }
                else
                {
                    keys[i].bReleased = true;
                    keys[i].bHeld = false;
                }
            }
            keys_old[i] = keys_new[i];
        }

        // Clear the inactive cache
        keyCache[nKeyCacheIndex ^ 0x01].clear();

        // Swap active cache index
        nKeyCacheIndex ^= 0x01;
    }
}
//! END IMPLEMENTATION
