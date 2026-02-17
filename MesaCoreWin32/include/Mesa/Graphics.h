#pragma once
#include "Core.h"
#include "Window.h"
#include "Exception.h"
#include "GfxUtils.h"
#include "GameObject.h"
#include "Camera.h"

namespace Mesa
{
	class MSAPI Graphics
	{
	public:
		Graphics();
		virtual ~Graphics();

		virtual void DrawFrame(Window* p_Window) = 0;
		virtual std::map<std::string, uint32_t> CompileForwardShaderPack(const std::string& packPath) = 0;
		virtual std::map<std::string, uint32_t> CompileDeferredShaderPack(const std::string& packPath) = 0;
		virtual std::map<std::string, uint32_t> LoadTexturePack(const std::string& packPath) = 0;
		virtual std::map<std::string, uint32_t> LoadModelPack(const std::string& packPath) = 0;
		virtual std::map<std::string, uint32_t> LoadMaterialPack(const std::string& packPath) = 0;
		virtual void SetNumberOfLayers(const uint32_t& layers) = 0;
		virtual void SetCamera(Camera* p_Camera) = 0;
		virtual void InsertGameObject(GameObject3D* p_GameObject) = 0;
		virtual uint32_t LoadModelFromPack(const std::string& originalName) = 0;
		virtual uint32_t CompileForwardShaderFromPack(const std::string& vertexName) = 0;
		virtual uint32_t LoadTextureFromPack(const std::string& originalName) = 0;
		virtual uint32_t LoadMaterialFromPack(const std::string& originalName) = 0;
		virtual void SetBlendingShader(uint32_t shaderId) = 0;
		virtual uint32_t LoadSourceModel(const std::string& originalName) = 0;
		virtual uint32_t LoadSourceTexture(const std::string& originalName) = 0;
	};

	class MSAPI GraphicsDx11Exception : public Exception
	{
	public:
		GraphicsDx11Exception(HRESULT hr, std::source_location loc = std::source_location::current());
		const char* what() const noexcept override;

	private:
		HRESULT m_Code;
	};

	class MSAPI GraphicsDx11 : public Graphics
	{
	public:
		GraphicsDx11(Window* p_Window);
		GraphicsDx11(HWND hWnd, uint32_t width, uint32_t height);
		~GraphicsDx11();

	public: // Frame drawing functions
		void DrawFrame(Window* p_Window) override;
		void SetNumberOfLayers(const uint32_t& layers) override;
		void SetCamera(Camera* p_Camera) override;
		void InsertGameObject(GameObject3D* p_GameObject) override;
		void SetBlendingShader(uint32_t shaderId) override;

	public: // Asset loading functions
		std::map<std::string, uint32_t> CompileForwardShaderPack(const std::string& packPath) override;
		std::map<std::string, uint32_t> CompileDeferredShaderPack(const std::string& packPath) override;
		std::map<std::string, uint32_t> LoadTexturePack(const std::string& packPath) override;
		std::map<std::string, uint32_t> LoadModelPack(const std::string& packPath) override;
		std::map<std::string, uint32_t> LoadMaterialPack(const std::string& packPath) override;

		uint32_t LoadModelFromPack(const std::string& originalName) override;
		uint32_t CompileForwardShaderFromPack(const std::string& vertexName) override;
		uint32_t LoadTextureFromPack(const std::string& originalName) override;
		uint32_t LoadMaterialFromPack(const std::string& originalName) override;
		uint32_t LoadSourceModel(const std::string& originalName) override;
		uint32_t LoadSourceTexture(const std::string& originalName) override;

	public: // Getters
		uint32_t GetShaderIdByVertexName(const std::string& name);
		uint32_t GetShaderIdByPixelName(const std::string& name);
		uint32_t GetTextureIdByName(const std::string& name);
		uint32_t GetModelIdByName(const std::string& name);
		uint32_t GetMaterialIdByName(const std::string& name);

	private: // Pipeline initialization functions
		void InitializeFactory();
		IDXGIAdapter* FindSuitableAdapter();
		void InitializeDevice();
		void InitializeSwapChain(HWND hWnd, uint32_t width, uint32_t height);
		void InitializeDepthBuffer(uint32_t width, uint32_t height);
		void InitializeDepthState();
		void InitializeDepthView();
		void InitializeRenderTargetView();
		void InitializeViewport(uint32_t width, uint32_t height);
		void InitializeRasterizer();
		void InitializeSampler();
		void InitializeBlendState();

	private: // Model data processing
		void ProcessNode(ModelDx11& outMdl, aiNode* p_Node, const aiScene* p_Scene);
		MeshDx11 ProcessMesh(aiMesh* p_Mesh, const aiScene* p_Scene);

	private: // Engine side assets initializers
		void InitializeBlendingMesh();

	private: // Rendering functions
		void RenderColorBuffer(int layer);
		void RenderSpecularBuffer(int layer);
		void BlendLayers();

	private: // Synchronus asset loading functions
		std::map<std::string, std::string> LoadMaterialDefinitions(const std::string& matDefName);

	private: // Asynchronus asset loading functions
		// Vertex buffer creation
		static void CreateVertexBuffer(std::vector<VertexDx11> v_verts, ID3D11Buffer** pp_Buffer, GraphicsDx11* p_Gfx, bool& result);
		static void CreateDeferredVertexBuffer(std::vector<DeferredVertexDx11> v_verts, ID3D11Buffer** pp_Buffer, GraphicsDx11* p_Gfx, bool& result);
		static void CreateVertexBufferCritical(std::vector<VertexDx11> v_verts, ID3D11Buffer** pp_Buffer, GraphicsDx11* p_Gfx);
		static void CreateDeferredVertexBufferCritical(std::vector<DeferredVertexDx11> v_verts, ID3D11Buffer** pp_Buffer, GraphicsDx11* p_Gfx);

		// Shader compilation
		static void CompileShader(std::vector<uint8_t> v_VertexData, std::vector<uint8_t> v_PixelData, ShaderType type, GraphicsDx11* p_Gfx, std::string vertexName, std::string pixelName);
		static void CompileVertexShader(std::vector<uint8_t> v_VertexData, ShaderType type, ID3D11VertexShader** pp_Shader, ID3D11InputLayout** pp_Layout, GraphicsDx11* p_Gfx);
		static void CompilePixelShader(std::vector<uint8_t> v_PixelData, ShaderType type, ID3D11PixelShader** pp_Shader, GraphicsDx11* p_Gfx);
		
		// Index buffer creation
		static void CreateIndexBuffer(std::vector<uint32_t> v_inds, ID3D11Buffer** pp_Buffer, GraphicsDx11* p_Gfx, bool& result);
		
		// Const buffer creation
		static void CreateEmptyBuffer(size_t size, UINT bindFlag, D3D11_USAGE usage, UINT cpuAccess, GraphicsDx11* p_Gfx, ID3D11Buffer** pp_Buffer, bool& result);
		static void CreateCriticalBuffer(size_t size, UINT bindFlag, D3D11_USAGE usage, UINT cpuAccess, GraphicsDx11* p_Gfx, ID3D11Buffer** pp_Buffer);
		
		// Texture loading
		static void LoadTexture(std::vector<uint8_t> v_TextureData, GraphicsDx11* p_Gfx, std::string textureName);
		static void LoadTextureFromPackAsync(std::string originalName, GraphicsDx11* p_Gfx);
		static void CreateCriticalTexture(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlag, GraphicsDx11* p_Gfx, ID3D11Texture2D** pp_Texture, ID3D11ShaderResourceView** pp_View);
		
		// Model loading
		static void LoadModel(std::vector<uint8_t> v_ModelData, GraphicsDx11* p_Gfx, std::string modelName);
		
		// Material loading
		static void CreateMaterial(std::vector<uint8_t> v_MatData, GraphicsDx11* p_Gfx, std::string matName);

	private: // ID generating functions
		uint32_t GenerateShaderUID();
		uint32_t GenerateTextureUID();
		uint32_t GenerateModelUID();
		uint32_t GenerateMaterialUID();

	private: // ID generators semaphores
		std::binary_semaphore m_TextureIdSemaphore{ 1 };
		std::binary_semaphore m_ShaderIdSemaphore{ 1 };
		std::binary_semaphore m_MaterialIdSemaphore{ 1 };
		std::binary_semaphore m_ModelIdSemaphore{ 1 };

	private: // Basic D3D11 pipeline interfaces
		Microsoft::WRL::ComPtr<IDXGIFactory> mp_Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter> mp_Adapter;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mp_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> mp_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mp_Context;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mp_DepthState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mp_DepthView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mp_RenderTarget;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> mp_RasterizerState;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mp_DepthBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mp_BackBuffer;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mp_Sampler;
		Microsoft::WRL::ComPtr<ID3D11BlendState> mp_BlendState;

	private: // Layer rendering interfaces
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mp_LayerColorBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mp_PrevLayerColorBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mp_ColorResourceView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mp_PrevColorResourceView;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> mp_LayerSpecBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mp_PrevLayerSpecBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mp_SpecResourceView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mp_PrevSpecResourceView;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mp_BlendingPlaneBuffer;

	private: // Buffers for loaded assets
		std::vector<ShaderDx11> mv_Shaders;
		std::vector<TextureDx11> mv_Textures;
		std::vector<ModelDx11> mv_Models;
		std::vector<Material> mv_Materials;

	private: // Vector to hold drawable game objects
		std::vector<GameObject3D*> mv_Objects;

	private: // Camera related data
		CameraDx11* mp_Camera = nullptr;

	private: // Data related to layer drawing
		uint32_t m_NumLayers = 1; // Use only 1 layer by default
		uint32_t m_BlendingShaderId = 0;
	};
}