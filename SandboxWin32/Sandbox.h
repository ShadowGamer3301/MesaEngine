#pragma once
#include <Mesa/Mesa.h>

class Sandbox : public Mesa::Application
{
public:
	Sandbox();
	~Sandbox();

	void Run() override;
	void ManageEvents();

private:
	Mesa::CameraDx11 m_Camera;
	Mesa::GameObject3D m_Object;
};