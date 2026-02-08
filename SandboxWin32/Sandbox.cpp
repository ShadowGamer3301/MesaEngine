#include "Sandbox.h"

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
}

void Sandbox::Run()
{
	while (mp_Window->Update())
	{

	}
}

Mesa::Application* Mesa::NewApplication()
{
	return new Sandbox();
}