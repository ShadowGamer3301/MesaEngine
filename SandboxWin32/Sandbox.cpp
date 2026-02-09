#include "Sandbox.h"

Sandbox::Sandbox()
{
	mp_Graphics->CompileForwardShaderPack("Forward_Shaders.msdp");
}

Sandbox::~Sandbox()
{
}

void Sandbox::Run()
{
	while (mp_Window->Update())
	{
		mp_Graphics->DrawFrame(mp_Window);
	}
}

Mesa::Application* Mesa::NewApplication()
{
	return new Sandbox();
}