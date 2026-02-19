#pragma once
#include "Core.h"

class EditorWindow
{
public:
	EditorWindow(uint32_t width, uint32_t height, const char* name);
	~EditorWindow();

	HWND GetNativeViewerHandle();

private:
	Fl_Window* p_Window = nullptr; // Main application window
	Fl_Window* p_ViewWindow = nullptr; // Material preview window

	Fl_Input* p_ModelInput = nullptr;
	Fl_Input* p_ColorShaderInput = nullptr;
	Fl_Input* p_SpecularShaderInput = nullptr;
	Fl_Input* p_NormalShaderInput = nullptr;
	Fl_Input* p_PositionShaderInput = nullptr;

	Fl_Button* p_ModelLoad = nullptr;
	Fl_Button* p_ColorLoad = nullptr;
	Fl_Button* p_SpecularLoad = nullptr;
};