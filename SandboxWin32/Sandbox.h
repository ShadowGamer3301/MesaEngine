#pragma once
#include <Mesa/Mesa.h>

class Sandbox : public Mesa::Application
{
public:
	Sandbox();
	~Sandbox();

	void Run() override;
};