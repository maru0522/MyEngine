#pragma once
#include <array>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#if _MSC_VER > 1922 && !defined(_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include <experimental/filesystem>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include "ConstBuffer.h"
#include "Window.h"
#include "GraphicsPipeline.h"

class Sprite
{
};

