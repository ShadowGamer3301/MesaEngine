#pragma once
#include "Core.h"
#include "EditorWindow.h"

class Editor
{
public:
	Editor();
	~Editor();

	void Run();
	void ManageEvents();

private:
	void GenerateMaterialFilesForModel(Mesa::ModelDx11* p_Model);
	void SaveMaterial(Mesa::Material* p_Material, std::string outPath);

private:
	EditorWindow* mp_EditorWindow = nullptr;
	bool m_ExitSignal = false;
	Mesa::GraphicsDx11* mp_Graphics = nullptr;
	Mesa::GameObject3D m_Object;

	std::string m_ModelName = std::string();
	std::string m_ColorPassName = std::string();
	std::string m_SpecularPassName = std::string();
};