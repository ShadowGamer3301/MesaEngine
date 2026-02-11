#include "Sandbox.h"

Sandbox::Sandbox()
{
	uint32_t shaderId = mp_Graphics->CompileForwardShaderFromPack("Asset_INT/Shader/V_WorldView.hlsl");
	if (shaderId == 0)
		throw Mesa::Exception();

	m_Object.SetColorShader(shaderId);

	uint32_t modelId = mp_Graphics->LoadModelFromPack("Asset_INT/Model/Jill_Stage_5.fbx");
	if (modelId == 0)
		throw Mesa::Exception();

	m_Object.SetModel(modelId);
	m_Object.SetRotation(glm::vec3(0, 180, 0));

	mp_Graphics->InsertGameObject(&m_Object);

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
		mp_Graphics->DrawFrame(mp_Window);
	}
}

Mesa::Application* Mesa::NewApplication()
{
	return new Sandbox();
}