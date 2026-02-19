#include "Editor.h"
#include "EditorEvent.h"

Editor::Editor()
{
	LOG_F(INFO, "Validating intermadiate directories...");

	BOOL createResult = 0;

	if (!Mesa::FileUtils::FileExists("Intermediate/Material"))
	{
		LOG_F(WARNING, "Intermediate/Material directory is missing! Creating directory...");

		createResult = CreateDirectory(L"Intermediate", nullptr);

		if (createResult == 0 && GetLastError() != ERROR_ALREADY_EXISTS) throw Mesa::Exception();

		createResult = CreateDirectory(L"Intermediate/Material", nullptr);

		if (createResult == 0 && GetLastError() != ERROR_ALREADY_EXISTS) throw Mesa::Exception();

		LOG_F(INFO, "Directories created!");
	}

	LOG_F(INFO, "Directories validated!");

	mp_EditorWindow = new EditorWindow(1280, 720, "MESA MatEditWin32");

	mp_Graphics = new Mesa::GraphicsDx11(mp_EditorWindow->GetNativeViewerHandle(), 800, 600);
}

Editor::~Editor()
{
	if (mp_Graphics) delete mp_Graphics;
	if (mp_EditorWindow) delete mp_EditorWindow;
}

void Editor::Run()
{
	while (!m_ExitSignal)
	{
		Fl::wait();
		ManageEvents();
		mp_Graphics->DrawFrame(mp_EditorWindow->GetNativeViewerHandle(), 800, 600);
	}
}

void Editor::ManageEvents()
{
	auto eb = Mesa::EventHandler::GetEventBuffer();

	for (const auto& event : eb)
	{
		if (event.first == EditorEventType_CloseWindow)
		{
			m_ExitSignal = true;
		}
		else if (event.first == EditorEventType_ModelTextUpdate)
		{
			TextUpdateEvent* p_Event = (TextUpdateEvent*)event.second;

			m_ModelName = p_Event->m_NewValue;
		}
		else if (event.first == EditorEventType_ModelLoad)
		{
			uint32_t modelId = mp_Graphics->LoadSourceModel(m_ModelName);
			if (modelId != 0) m_Object.SetModel(modelId);

			std::string matDefPath = m_ModelName + ".matdef";

			if (!Mesa::FileUtils::FileExists(matDefPath))
			{
				LOG_F(INFO, ".matdef file not found! Generating new one...");
				Mesa::FileUtils::MakeFile(matDefPath);

				LOG_F(INFO, ".matdef file generated!");
			}

			LOG_F(INFO, "Generating material files...");
			GenerateMaterialFilesForModel(mp_Graphics->GetModelById(modelId));
			mp_Graphics->RescanMaterialsSource();
		}
		else if (event.first == EditorEventType_ColorPassUpdate)
		{
			TextUpdateEvent* p_Event = (TextUpdateEvent*)event.second;
			m_ColorPassName = p_Event->m_NewValue;
		}
		else if (event.first == EditorEventType_ColorPassLoad)
		{
			uint32_t shaderId = mp_Graphics->CompileForwardShaderFromPack(m_ColorPassName);
			if (shaderId != 0) m_Object.SetColorShader(shaderId);
		}
		else if (event.first == EditorEventType_SpecularPassUpdate)
		{
			TextUpdateEvent* p_Event = (TextUpdateEvent*)event.second;
			m_SpecularPassName = p_Event->m_NewValue;
		}
		else if (event.first == EditorEventType_SpecularPassLoad)
		{
			uint32_t shaderId = mp_Graphics->CompileForwardShaderFromPack(m_SpecularPassName);
			if (shaderId != 0) m_Object.SetSpecularShader(shaderId);
		}
	}

	Mesa::EventHandler::ClearEventBuffer();
}

void Editor::GenerateMaterialFilesForModel(Mesa::ModelDx11* p_Model)
{
	size_t numMeshes = p_Model->GetNumMeshes();
	std::string modelName = p_Model->GetModelName();
	std::string matDef = modelName + ".matdef";

	for (size_t i = 0; i < numMeshes - 1; i++)
	{
		Mesa::MeshDx11* p_Mesh = p_Model->GetMesh(i);

		if (p_Mesh == nullptr) continue;

		std::string matDir = "Intermediate/Material/";

		std::string matPath = Mesa::FileUtils::CombinePaths(matDir, p_Mesh->GetMaterialFileName());
		matPath += ".mmat";

		Mesa::FileUtils::MakeFile(matPath);

		p_Mesh->SetFullMaterialName(matPath);

		Mesa::Material defaultMat;

		if(Mesa::FileUtils::FileSize(matPath) == 0)
			SaveMaterial(&defaultMat, matPath);

		std::string meshMat = p_Mesh->GetMaterialFileName();

		std::string materialDefinition = meshMat + "=" + matPath + "\n";
		Mesa::FileUtils::AppendTextToFile(matDef, materialDefinition);
	}
}

void Editor::SaveMaterial(Mesa::Material* p_Material, std::string outPath)
{
	std::ostringstream oss;

	glm::vec4 baseColor = p_Material->GetBaseColor();
	glm::vec4 subColor = p_Material->GetSubColor();

	oss << "$base_r " << baseColor.r << "\n"
		<< "$base_g " << baseColor.g << "\n"
		<< "$base_b " << baseColor.b << "\n"
		<< "$base_a " << baseColor.a << "\n"
		<< "$sub_r " << subColor.r << "\n"
		<< "$sub_g " << subColor.g << "\n"
		<< "$sub_b " << subColor.b << "\n"
		<< "$sub_a " << subColor.a << "\n"
		<< "$specular " << p_Material->GetSpecularPower() << "\n";

	std::string matData = oss.str();

	if (p_Material->GetDiffuseTextureId() != 0)
	{
		Mesa::TextureDx11* p_Texture = mp_Graphics->GetTextureById(p_Material->GetDiffuseTextureId());
		std::string texName = p_Texture->GetTextureName();

		matData = matData + texName + "\n";
	}

	if (p_Material->GetNormalTextureId() != 0)
	{
		Mesa::TextureDx11* p_Texture = mp_Graphics->GetTextureById(p_Material->GetNormalTextureId());
		std::string texName = p_Texture->GetTextureName();

		matData = matData + texName + "\n";
	}

	if (p_Material->GetSpecularTextureId() != 0)
	{
		Mesa::TextureDx11* p_Texture = mp_Graphics->GetTextureById(p_Material->GetSpecularTextureId());
		std::string texName = p_Texture->GetTextureName();

		matData = matData + texName + "\n";
	}

	std::vector<uint8_t> v_MatData(matData.size());

	memcpy(&v_MatData[0], &matData[0], sizeof(char) * matData.size());

	Mesa::FileUtils::AppendDataToFile(outPath, v_MatData);
}
 