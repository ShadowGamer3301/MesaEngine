#include "EditorWindow.h"
#include "EditorCallbacks.h"

EditorWindow::EditorWindow(uint32_t width, uint32_t height, const char* name)
{
	p_Window = new Fl_Window(width, height, name);
	if (!p_Window) throw Mesa::Exception();
	p_Window->callback(WindowCloseCallback);

	p_ViewWindow = new Fl_Window(10, 10, 800, 600, nullptr);
	if (!p_ViewWindow) throw Mesa::Exception();
	p_ViewWindow->show();
	p_ViewWindow->end();

	p_ModelInput = new Fl_Input(970, 10, 200, 40, "Model: ");
	p_ModelInput->callback(ModelTextCallback);

	p_ColorShaderInput = new Fl_Input(970, 50, 200, 40, "Color pass shader: ");
	p_ColorShaderInput->callback(ColorTextCallback);

	p_SpecularShaderInput = new Fl_Input(970, 90, 200, 40, "Color pass shader: ");
	p_SpecularShaderInput->callback(SpecularTextCallback);

	p_ModelLoad = new Fl_Button(1180, 10, 50, 40, "Load");
	p_ModelLoad->callback(ModelLoadCallback);

	p_ColorLoad = new Fl_Button(1180, 50, 50, 40, "Load");
	p_ColorLoad->callback(ColorLoadCallback);

	p_SpecularLoad = new Fl_Button(1180, 90, 50, 40, "Load");
	p_SpecularLoad->callback(SpecularLoadCallback);

	p_Window->end();
	p_Window->show();
}

EditorWindow::~EditorWindow()
{
	if (p_ModelLoad) delete p_ModelLoad;
	if (p_ColorLoad) delete p_ColorLoad;
	if (p_SpecularLoad) delete p_SpecularLoad;

	if (p_ModelInput) delete p_ModelInput;
	if (p_ColorShaderInput) delete p_ColorShaderInput;
	if (p_SpecularShaderInput) delete p_SpecularShaderInput;

	if (p_ViewWindow) delete p_ViewWindow;

	if (p_Window) delete p_Window;
}

HWND EditorWindow::GetNativeViewerHandle()
{
	return fl_xid(p_ViewWindow);
}
