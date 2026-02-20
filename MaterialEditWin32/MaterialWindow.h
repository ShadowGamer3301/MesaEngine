#pragma once
#include "Core.h"

class MaterialWindow
{
public:
	MaterialWindow(uint32_t width, uint32_t height, const char* name);
	~MaterialWindow();

private:
	Fl_Window* p_Window = nullptr;
};