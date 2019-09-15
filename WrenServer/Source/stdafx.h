// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// winsock headers need to be included before windows.h
#include <winsock2.h>
#include <Ws2tcpip.h>

// Windows Header Files
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <sqlite3.h>
#include <Combaseapi.h>
#include <sodium.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <list>
#include <queue>
#include <map>
#include <DirectXMath.h>
#include <random>
#include <Extensions.h>
#include <functional>

using namespace DirectX;
