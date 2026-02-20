#include "MaterialWindow.h"
#include "EditorCallbacks.h"

MaterialWindow::MaterialWindow(uint32_t width, uint32_t height, const char* name)
{
	p_Window = new Fl_Window(width, height, name);
	if (!p_Window) throw Mesa::Exception();
	p_Window->callback(WindowCloseCallback);

	p_Dropdown = new Fl_Choice(10, 10, 380, 40);

	p_Window->end();
	p_Window->show();
}

MaterialWindow::~MaterialWindow()
{
	if (p_Dropdown) delete p_Dropdown;

	if (p_Window) delete p_Window;
}

void MaterialWindow::FillDropdown(const std::vector<std::string>& v_Values)
{
	for (const auto& value : v_Values)
	{
		p_Dropdown->add(value.c_str());
	}
}
