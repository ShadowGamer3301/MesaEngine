#include "MaterialWindow.h"
#include "EditorCallbacks.h"

MaterialWindow::MaterialWindow(uint32_t width, uint32_t height, const char* name)
{
	p_Window = new Fl_Window(width, height, name);
	if (!p_Window) throw Mesa::Exception();
	p_Window->callback(WindowCloseCallback);

	p_Window->end();
	p_Window->show();
}

MaterialWindow::~MaterialWindow()
{
	if (p_Window) delete p_Window;
}
