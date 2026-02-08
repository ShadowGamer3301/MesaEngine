#pragma once

#ifdef _WINDLL
	#define MSAPI __declspec(dllexport)
#else
	#define MSAPI __declspec(dllimport)
#endif

// Windows related macros
#define WIN32_LEAN_AND_MEAN // Disable additional WIN32 functionality
#define GLFW_EXPOSE_NATIVE_WIN32 // Expose GLFW library to native WINAPI interfaces

// Windows related headers
#include <Windows.h>
#include <wrl.h>

// C++ standard library headers
#include <cstdint>
#include <sstream>
#include <source_location>
#include <filesystem>
#include <exception>
#include <vector>
#include <thread>
#include <array>
#include <fstream>
#include <map>
#include <set>
#include <optional>
#include <limits>
#include <algorithm>
#include <random>
#include <semaphore>

// GLFW headers
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// DirectX 11 headers
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// mINI headers
#include <mini/ini.h>

// Lodepng headers
#include <lodepng.h>

// GLM headers
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// Assimp Headers
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Loguru headers
#include <loguru/loguru.hpp>

// Crc32c headers
#include <crc32c/crc32c.h>

// LZAV headers
#include <lzav.h>