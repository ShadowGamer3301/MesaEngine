#pragma once
#include "Core.h"
#include "Exception.h"

namespace Mesa
{
	class MSAPI WindowException : Exception
	{
	public:
		WindowException(int code, std::source_location loc = std::source_location::current());
		const char* what() const noexcept override;

	private:
		int m_Code;
	};

	class MSAPI Window
	{
	private:
		static void WindowKeyCallback(GLFWwindow* p_Window, int key, int scancode, int action, int mods);
		static void WindowCharCallback(GLFWwindow* p_Window, unsigned int codepoint);
		static void WindowCursorPosCallback(GLFWwindow* p_Window, double xpos, double ypos);
		static void WindowCursorEnterCallback(GLFWwindow* p_Window, int entered);
		static void WindowMouseBtnCallback(GLFWwindow* p_Window, int button, int action, int mods);
		static void WindowScrollCallback(GLFWwindow* p_Window, double xoffset, double yoffset);
		static void GamepadCallback(int jid, int event);
	public:
		Window(uint32_t width, uint32_t height, const char* title, bool fullscreen);
		~Window();

		bool Update();
		HWND GetNativeWindow();
		uint32_t GetWindowWidth() noexcept;
		uint32_t GetWindowHeight() noexcept;
		inline bool IsFullscreen() const noexcept { return m_Fullscreen; }
		void SetGamepadStatus(const int& gpId, const bool& connected);

	private:
		void QueryConnectedGamepads();
		void UpdateGamepadStatus();

	private:
		GLFWwindow* mp_Window = nullptr;
		bool m_Fullscreen = false;
		std::map<int, bool> m_GamepadsMap;
	};
}