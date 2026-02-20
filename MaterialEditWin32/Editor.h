#pragma once
#include "Core.h"
#include "EditorWindow.h"
#include "MaterialWindow.h"

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
	EditorWindow* mp_EditorWindow = nullptr; // Main editor window
	MaterialWindow* mp_MaterialWindow = nullptr; // Material property editor window
	bool m_ExitSignal = false; // Singnal used for determining if the application should finish
	Mesa::GraphicsDx11* mp_Graphics = nullptr; // Graphics interface used for preview
	Mesa::GameObject3D m_Object; // Preview object
	Mesa::CameraDx11 m_Camera; // Preview camera

	std::string m_ModelName = std::string(); // Holds texts that is in model path input
	std::string m_ColorPassName = std::string(); // Holds texts that is in color pass path input
	std::string m_SpecularPassName = std::string(); // Holds texts that is in specular pass path input
};