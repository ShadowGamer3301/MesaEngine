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
	auto eb = Mesa::EventHandler::GetEventBuffer();

	for (const auto& ev : eb)
	{
		if (ev.first == Mesa::EventType_GamepadUpdate)
		{
			Mesa::GamepadUpdateEvent* p_Event = (Mesa::GamepadUpdateEvent*)ev.second;

			m_Camera.HandleMovement(Mesa::CameraMovementBackward, p_Event->ma_Axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * 0.01f);
			m_Camera.HandleMovement(Mesa::CameraMovementRight, p_Event->ma_Axes[GLFW_GAMEPAD_AXIS_LEFT_X] * 0.01f);

		}
	}

	Mesa::EventHandler::ClearEventBuffer();
}

Mesa::Application* Mesa::NewApplication()
{
	return new Sandbox();
}