#pragma once
#include "Core.h"
#include "Window.h"
#include "Graphics.h"

namespace Mesa
{
	class MSAPI Application
	{
	public:
		Application();
		~Application();

		virtual void Run() = 0;

	protected:
		Window* mp_Window = nullptr;
		Graphics* mp_Graphics = nullptr;
	};

	// Needs to be defined in SandboxWin32
	Application* NewApplication();
}