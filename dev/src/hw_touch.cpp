#include "hw_touch.h"

#include <algorithm>

//! START IMPLEMENTATION
namespace olc::hw
{
    const Button& Touch::GetTouch(const int nTouch) const
    {
        auto ids = GetTouchIDs();
        auto id = std::find(ids.begin(), ids.end(), nTouch);
        if (id == ids.end())
        {
            static Button emptyButton{};
            return emptyButton;
        }
        
        return touches.at(*id);
    }

    const olc::vf2d &Touch::GetPosition(const int nTouch) const
    {
        auto ids = GetTouchIDs();
        auto id = std::find(ids.begin(), ids.end(), nTouch);
        if (id == ids.end())
        {
            static olc::vf2d emptyPosition{};
            return emptyPosition;
        }
        
        return positions.at(*id);
    }

    uint32_t Touch::GetActiveTouches() const
    {
        return nActiveTouches;
    }

    void Touch::SetPosition(const int nTouch, const olc::vf2d &pos)
    {
        auto it = positions.find(nTouch);
        if (it == positions.end())
        {
            positions.emplace(nTouch, pos);
            touches_new.emplace(nTouch, false);
            touches_old.emplace(nTouch, false);
            touches.emplace(nTouch, Button{});
        }
        else
        {
            it->second = pos;
        }
    }

    void Touch::SetTouch(const int nTouch, bool state)
    {
        auto it = touches_new.find(nTouch);
        if (it == touches_new.end())
        {
            touches_new.emplace(nTouch, state);
            touches_old.emplace(nTouch, false);
            touches.emplace(nTouch, Button{});
            positions.emplace(nTouch, olc::vf2d{});
        }
        else
        {
            it->second = state;
        }
    }

    void Touch::UpdateState()
    {
        for (auto& [i, btn] : touches)
        {
            btn.bPressed = false;
            btn.bReleased = false;
            
            if (touches_new.at(i) != touches_old.at(i))
            {
                if (touches_new.at(i))
                {
                    btn.bPressed = !btn.bHeld;
                    btn.bHeld = true;
                }
                else
                {
                    btn.bReleased = true;
                    btn.bHeld = false;
                }
            }
            touches_old.at(i) = touches_new.at(i);
        }
    }

    void Touch::SetActiveTouches(const uint32_t nTouches)
    {
        nActiveTouches = nTouches;
    }
    
    std::vector<int32_t> Touch::GetTouchIDs() const
    {
        std::vector<int32_t> ids;
        ids.reserve(touches.size());
        for (const auto& [id, _] : touches)
        {
            ids.push_back(id);
        }
        return ids;
    }
}
//! END IMPLEMENTATION