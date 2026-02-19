#include "Editor.h"
#include "EditorEvent.h"

/*
	Constructor. Initializes all core components of MaterialEdit
*/
Editor::Editor()
{
	LOG_F(INFO, "Validating intermadiate directories...");

	BOOL createResult = 0;

	// Check if required directories already exist
	if (!Mesa::FileUtils::FileExists("Intermediate/Material"))
	{
		// If some directory is missing proceed to create them
		LOG_F(WARNING, "Intermediate/Material directory is missing! Creating directory...");

		createResult = CreateDirectory(L"Intermediate", nullptr);

		// Check if creation was succesfull but ignore error code that occurs when directory already exists
		if (createResult == 0 && GetLastError() != ERROR_ALREADY_EXISTS) throw Mesa::Exception();

		createResult = CreateDirectory(L"Intermediate/Material", nullptr);

		if (createResult == 0 && GetLastError() != ERROR_ALREADY_EXISTS) throw Mesa::Exception();

		LOG_F(INFO, "Directories created!");
	}

	LOG_F(INFO, "Directories validated!");

	// Initialize main editor window
	mp_EditorWindow = new EditorWindow(1280, 720, "MESA MatEditWin32");

	// Initialize graphics interface
	mp_Graphics = new Mesa::GraphicsDx11(mp_EditorWindow->GetNativeViewerHandle(), 800, 600);
}

/*
	Destructor. Ensures that the program frees all interfaces correctly
*/
Editor::~Editor()
{
	if (mp_Graphics) delete mp_Graphics;
	if (mp_EditorWindow) delete mp_EditorWindow;
}

/*
	Main loop of the application
*/
void Editor::Run()
{
	// Check if we recieved exit signal
	while (!m_ExitSignal)
	{
		// Update all FLTK widgets
		Fl::wait();
		// Manage incoming events
		ManageEvents();
		// Redraw frame in preview widget
		mp_Graphics->DrawFrame(mp_EditorWindow->GetNativeViewerHandle(), 800, 600);
	}
}

/*
	Manages all incoming events
*/
void Editor::ManageEvents()
{
	// Obtain event buffer
	auto eb = Mesa::EventHandler::GetEventBuffer();

	// Go through every event
	for (const auto& event : eb)
	{
		// If the main window wass closed
		if (event.first == EditorEventType_CloseWindow)
		{
			// Enable exit signal
			m_ExitSignal = true;
		}
		// If data in model input was updated
		else if (event.first == EditorEventType_ModelTextUpdate)
		{
			// Obtain new value of model input and save it
			TextUpdateEvent* p_Event = (TextUpdateEvent*)event.second;
			m_ModelName = p_Event->m_NewValue;
		}
		// If model load button was pressed
		else if (event.first == EditorEventType_ModelLoad)
		{
			// Load new model from source file
			uint32_t modelId = mp_Graphics->LoadSourceModel(m_ModelName);
			// If model was loaded correctly set it as our preview object
			if (modelId != 0) m_Object.SetModel(modelId);

			// Create path for matdef file
			std::string matDefPath = m_ModelName + ".matdef";

			// Check if matdef file already exists
			if (!Mesa::FileUtils::FileExists(matDefPath))
			{
				// If not generate new .matdef file
				LOG_F(INFO, ".matdef file not found! Generating new one...");
				Mesa::FileUtils::MakeFile(matDefPath);
				LOG_F(INFO, ".matdef file generated!");
			}

			LOG_F(INFO, "Generating material files...");
			// Start creating materials and material definitions
			GenerateMaterialFilesForModel(mp_Graphics->GetModelById(modelId));
			// Rescan for newly created or updated materials
			mp_Graphics->RescanMaterialsSource();
		}
		// If data in color pass input was changed
		else if (event.first == EditorEventType_ColorPassUpdate)
		{
			// Obtain new value of color pass input and save it
			TextUpdateEvent* p_Event = (TextUpdateEvent*)event.second;
			m_ColorPassName = p_Event->m_NewValue;
		}
		// If color pass load button was pressed
		else if (event.first == EditorEventType_ColorPassLoad)
		{
			// Compile color pass shader
			uint32_t shaderId = mp_Graphics->CompileForwardShaderFromPack(m_ColorPassName);
			// If compilation ended succesfully used it for our preview object
			if (shaderId != 0) m_Object.SetColorShader(shaderId);
		}
		// If data in specular pass input was changed
		else if (event.first == EditorEventType_SpecularPassUpdate)
		{
			// Obtain new value of specular pass input and save it
			TextUpdateEvent* p_Event = (TextUpdateEvent*)event.second;
			m_SpecularPassName = p_Event->m_NewValue;
		}
		// If color specular load button was pressed
		else if (event.first == EditorEventType_SpecularPassLoad)
		{
			// Compile specular pass shader
			uint32_t shaderId = mp_Graphics->CompileForwardShaderFromPack(m_SpecularPassName);
			// If compilation ended succesfully used it for our preview object
			if (shaderId != 0) m_Object.SetSpecularShader(shaderId);
		}
	}

	// Clear event buffer
	Mesa::EventHandler::ClearEventBuffer();
}

/*
	Creates missing material files and fills out matdef file
*/
void Editor::GenerateMaterialFilesForModel(Mesa::ModelDx11* p_Model)
{
	// Grab needed data from model
	size_t numMeshes = p_Model->GetNumMeshes(); // Number of meshes in model
	std::string modelName = p_Model->GetModelName(); // Models name
	std::string matDef = modelName + ".matdef"; // Matdef file

	// Loop through every mesh in the model
	for (size_t i = 0; i < numMeshes - 1; i++)
	{
		// Try to obtain pointer to the mesh
		Mesa::MeshDx11* p_Mesh = p_Model->GetMesh(i);
		// Validate pointer
		if (p_Mesh == nullptr) continue;

		// Calculate path for material file
		std::string matDir = "Intermediate/Material/";
		std::string matPath = Mesa::FileUtils::CombinePaths(matDir, p_Mesh->GetMaterialFileName());
		matPath += ".mmat";

		// Check if material file already exists and if not create new one
		if(!Mesa::FileUtils::FileExists(matPath))
			Mesa::FileUtils::MakeFile(matPath);

		// Point mesh to newly generated file
		p_Mesh->SetFullMaterialName(matPath);

		Mesa::Material defaultMat;

		// If material file is empty fill it with default data
		if(Mesa::FileUtils::FileSize(matPath) == 0)
			SaveMaterial(&defaultMat, matPath);

		// Add file to material definition
		std::string meshMat = p_Mesh->GetMaterialFileName();
		std::string materialDefinition = meshMat + "=" + matPath + "\n";
		Mesa::FileUtils::AppendTextToFile(matDef, materialDefinition);
	}
}

/*
	Saves material to specifed file
*/
void Editor::SaveMaterial(Mesa::Material* p_Material, std::string outPath)
{
	std::ostringstream oss;

	glm::vec4 baseColor = p_Material->GetBaseColor();
	glm::vec4 subColor = p_Material->GetSubColor();

	// Save base and sub color values
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

	// If texture is conneted with material save path to it
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

	// Override already existing data with newly generated one
	std::vector<uint8_t> v_MatData(matData.size());
	memcpy(&v_MatData[0], &matData[0], sizeof(char) * matData.size());
	Mesa::FileUtils::MakeFileWithContent(outPath, v_MatData);
}
 