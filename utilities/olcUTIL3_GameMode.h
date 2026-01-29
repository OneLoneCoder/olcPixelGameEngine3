/*
	OneLoneCoder - GameMode v3.0
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	A very simple state machine base class for game states/modes that
	enforces a pattern javidx9 likes :D


	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2026 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019 - 2026

	Version
	~~~~~~~

	3.0		+Initial Release

*/

#pragma once

#include <olcPixelGameEngine3.h>

namespace olc::utils::gsm
{
	template<typename GData, typename GState>
	class Mode
	{
	public:

		Mode() = default;


		Mode(std::shared_ptr<GData> pGlobalData) : m_pGlobalData(pGlobalData)
		{

		}

	public:
		virtual bool OnCreate(olc::PixelGameEngine* pge)
		{
			return false;
		}

		virtual GState OnUpdate(olc::PixelGameEngine* pge, float fElapsedTime)
		{
			return GState{};
		}

		virtual bool OnEnterMode(olc::PixelGameEngine* pge)
		{
			return false;
		}

		virtual bool OnExitMode(olc::PixelGameEngine* pge)
		{
			return false;
		}

	protected:
		std::shared_ptr<GData> m_pGlobalData;
	};

}