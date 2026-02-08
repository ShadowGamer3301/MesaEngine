#pragma once
#include "Core.h"
#include "Window.h"

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
	};

	// Needs to be defined in SandboxWin32
	Application* NewApplication();
}