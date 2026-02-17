#include "Sandbox.h"

Sandbox::Sandbox()
{


	m_Camera.SetProjectionValues(60, mp_Window->GetWindowWidth()/(float)mp_Window->GetWindowHeight(), 0.01f, 1000.0f);
	mp_Graphics->SetCamera(&m_Camera);
}

Sandbox::~Sandbox()
{
}

void Sandbox::Run()
{
	while (mp_Window->Update())
	{
		ManageEvents();
		mp_Graphics->DrawFrame(mp_Window);
	}
}

void Sandbox::ManageEvents()
{
	Mesa::EventHandler::ClearEventBuffer();
}

Mesa::Application* Mesa::NewApplication()
{
	return new Sandbox();
}