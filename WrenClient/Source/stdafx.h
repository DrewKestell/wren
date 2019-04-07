// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <windowsx.h>

#include <string>
#include <iostream>
#include <exception>
#include <sstream>
#include <fstream>  
#include <vector>
#include <algorithm>
#include <functional>
#include <codecvt>
#include <list>
#include <forward_list>
#include <queue>
#include <d3d11.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h"

using namespace DirectX;