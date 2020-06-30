#pragma once

#include "targetver.h"

#include <vector>
#include <string>
#include <memory>
#include <functional>

#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <d3d9.h>
#include <shellapi.h>

#include "../dep/gw2al_api.h"
#include "../dep/gw2al_d3d9_wrapper.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <examples/imgui_impl_dx9.h>
#include <examples/imgui_impl_win32.h>

#include "Singleton.h"
#include "Core.h"
#include "Direct3D9Hooks.h"

#define COM_RELEASE(x) { if((x)) { (x)->Release(); (x) = nullptr; } }
#define NULL_COALESCE(a, b) ((a) != nullptr ? (a) : (b))
#define SQUARE(x) ((x) * (x))

typedef unsigned char uchar;
typedef unsigned int uint;
typedef std::basic_string<TCHAR> tstring;
typedef unsigned __int64 mstime;

