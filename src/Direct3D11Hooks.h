#pragma once
#include "stdafx.h"

namespace lib_imgui
{

	class Direct3D11Hooks : public Singleton<Direct3D11Hooks>
	{
	public:
		Direct3D11Hooks();

		void InitHooks(gw2al_core_vtable* gAPI);
	};

}
