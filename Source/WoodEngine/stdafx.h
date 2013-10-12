// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>




// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cassert>
#include <math.h>
#include <assert.h>

#include "libs\glew.h"

#define REQUIRES(x) assert(x); 
#define PROMISES(x) assert(x);

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")


// TODO: reference additional headers your program requires here


#ifndef STBI_HEADER_FILE_ONLY
#define STBI_HEADER_FILE_ONLY
#include "Engine\stb_image.c"
#endif

#include "Math\Vector3.hpp"
#include "Math\Vector2.hpp"
#include "Utility\Utility.hpp"

const woodman::Vector2i ScreenSize(1600, 900);
const float g_MaxLayer(2000);
const std::string ASSETS("..\\..\\Assets\\");
const std::string TEXTURES_PATH(ASSETS + "Textures\\");





