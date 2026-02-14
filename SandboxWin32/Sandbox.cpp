#include "Sandbox.h"

Sandbox::Sandbox()
{
	uint32_t shaderId = mp_Graphics->CompileForwardShaderFromPack("Asset_INT/Shader/V_ColorPass.hlsl");
	if (shaderId == 0)
		throw Mesa::Exception();

	m_Object.SetColorShader(shaderId);

	uint32_t modelId = mp_Graphics->LoadModelFromPack("Asset_INT/Model/Jill_Stage_5.fbx");
	if (modelId == 0)
		throw Mesa::Exception();

	m_Object.SetModel(modelId);
	m_Object.SetRotation(glm::vec3(0, 180, 0));

	mp_Graphics->InsertGameObject(&m_Object);

	uint32_t textureId = mp_Graphics->LoadTextureFromPack("Asset_INT/Texture/Chara000_DM_HQ.png");
	if (textureId == 0)
		throw Mesa::Exception();

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