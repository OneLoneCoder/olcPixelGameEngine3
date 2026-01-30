#pragma once

//! START STDHEADER
#include <unordered_map>
#include <vector>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "hw_input.h"
#include "vector2d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_HW_TOUCH_DECLARED)
namespace olc
{
    // Forward declare for friendship
	class Window;
	class PGEWindow;

    namespace hw
    {
        class Touch
        {
            friend class olc::Window;
            friend class olc::PGEWindow;
        public:
            Touch() = default;

        public:
            const Button& GetTouch(const int nTouch) const;
            const olc::vf2d& GetPosition(const int nTouch) const;
            uint32_t GetActiveTouches() const;

        protected:
            std::unordered_map<int32_t, Button> touches{};
            std::unordered_map<int32_t, olc::vf2d> positions{};
            std::unordered_map<int32_t, bool> touches_new{};
            std::unordered_map<int32_t, bool> touches_old{};
            uint32_t nActiveTouches = 0;

            std::vector<int32_t> GetTouchIDs() const;

        private:
            void SetPosition(const int nTouch, const olc::vf2d& pos);
            void SetTouch(const int nTouch, bool state);
            void SetActiveTouches(const uint32_t nTouches);
            void UpdateState();
        };
    }
}
#endif