// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define NOMINMAX

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
#include <wrl/client.h>

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
#include <dxgi1_6.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "DDSTextureLoader.h"
#include <cmath>
#include <memory>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		virtual const char* what() const override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}
}