#include "Sandbox.h"

Sandbox::Sandbox()
{
	auto shaderpack = mp_Graphics->CompileForwardShaderPack("Forward_Shaders.msdp");
	mp_Graphics->LoadTexturePack("testTextures.mtp");
	auto mdlpack = mp_Graphics->LoadModelPack("testModel.mmdp");

	m_Camera.SetProjectionValues(60, mp_Window->GetWindowWidth()/(float)mp_Window->GetWindowHeight(), 0.01f, 1000.0f);

	for (auto& model : mdlpack)
	{
		m_Object.SetModel(model.second);
	}

	for (auto& shader : shaderpack)
	{
		m_Object.SetColorShader(shader.second);
	}
	m_Object.SetRotation(glm::vec3(0, 180, 0));
	mp_Graphics->InsertGameObject(&m_Object);
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