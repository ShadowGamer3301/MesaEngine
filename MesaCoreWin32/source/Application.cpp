#include <Mesa/Application.h>
#include <Mesa/ConvertUtils.h>
#include <Mesa/ConfigUtils.h>

namespace Mesa
{
	/*
		Constructor: Initializes the application by reading configuration settings,
		validating window dimensions, and setting up the window and graphics API.
	*/
	Application::Application()
	{
		LOG_F(INFO, "Starting Mesa application...");

		// Retrieve window dimensions from engine.ini.
		int windowWidth = ConvertUtils::StringToInt(ConfigUtils::GetValueFromConfig("Window", "Width"));
		int windowHeight = ConvertUtils::StringToInt(ConfigUtils::GetValueFromConfig("Window", "Height"));

		// Ensure the window isn't too small to display content properly.
		if (windowWidth < 800) windowWidth = 800;
		if (windowHeight < 600) windowHeight = 600;

		LOG_F(INFO, "Perparing window creation...");
		LOG_F(INFO, "Window width set to %i px", windowWidth);
		LOG_F(INFO, "Window height set to %i px", windowHeight);

		// Check for fullscreen mode.
		bool fullscreen = strcmp(ConfigUtils::GetValueFromConfig("Window", "Fullscreen").c_str(), "true") == 0;

		// Create the OS-level window instance.
		mp_Window = new Window(windowWidth, windowHeight, "SandboxWin32", fullscreen);
		// Initialize DX11 renderer
		mp_Graphics = new GraphicsDx11(mp_Window);
	}

	/*
		Destructor: Cleans up dynamically allocated memory to prevent memory leaks.
	*/
	Application::~Application()
	{
		if (mp_Graphics) delete mp_Graphics;
		if (mp_Window) delete mp_Window;
	}
}