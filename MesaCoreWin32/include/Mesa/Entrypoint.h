#pragma once
#include "Application.h"
#include "Exception.h"
#include "FileUtils.h"
#include "ConfigUtils.h"

extern Mesa::Application* Mesa::NewApplication();

#ifdef _DEBUG

int main() try
{
	if (!Mesa::FileUtils::FileExists("engine.ini"))
		Mesa::ConfigUtils::GenerateConfig();

	auto app = Mesa::NewApplication();
	
	app->Run();
	
	delete app;

	return 0;
}
catch (Mesa::Exception& e)
{
	LOG_F(ERROR, "%s", e.what());
	return 1;
}

#else

#endif