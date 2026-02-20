#pragma once
#include "Core.h"

class MaterialWindow
{
public:
	MaterialWindow(uint32_t width, uint32_t height, const char* name);
	~MaterialWindow();

	void FillDropdown(const std::vector<std::string>& v_Values);

private:
	Fl_Window* p_Window = nullptr;

	Fl_Choice* p_Dropdown = nullptr;
};