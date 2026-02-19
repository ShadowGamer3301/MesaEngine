#include "Editor.h"

int main(void) try
{
	if (!Mesa::FileUtils::FileExists("engine.ini"))
		Mesa::ConfigUtils::GenerateConfig();

	Editor* p_Editor = new Editor();

	p_Editor->Run();

	if (p_Editor) delete p_Editor;

	return 0;
}
catch (Mesa::Exception& me)
{
	LOG_F(ERROR, "%s", me.what());
	return 1;
}