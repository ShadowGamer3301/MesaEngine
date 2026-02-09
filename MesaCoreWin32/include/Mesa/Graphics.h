#pragma once
#include "Core.h"
#include "Window.h"
#include "Exception.h"
#include "GfxUtils.h"

namespace Mesa
{
	class MSAPI Graphics
	{
	public:
		Graphics();
		virtual ~Graphics();

		virtual void DrawFrame(Window* p_Window) = 0;
		virtual std::map<std::string, uint32_t> CompileForwardShaderPack(const std::string& packPath) = 0;
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
		~GraphicsDx11();

	public: // Frame drawing functions
		void DrawFrame(Window* p_Window);

	public: // Asset loading functions
		std::map<std::string, uint32_t> CompileForwardShaderPack(const std::string& packPath);
		std::map<std::string, uint32_t> CompileDeferredShaderPack(const std::string& packPath);

	public: // Getters
		uint32_t GetShaderIdByVertexName(const std::string& name);
		uint32_t GetShaderIdByPixelName(const std::string& name);

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

	private: // Rendering functions
		void RenderColorBuffer();

	private: // Asynchronus asset loading functions
		static void CompileShader(std::vector<uint8_t> v_VertexData, std::vector<uint8_t> v_PixelData, ShaderType type, GraphicsDx11* p_Gfx, std::string vertexName, std::string pixelName);
		static void CompileVertexShader(std::vector<uint8_t> v_VertexData, ShaderType type, ID3D11VertexShader** pp_Shader, ID3D11InputLayout** pp_Layout, GraphicsDx11* p_Gfx);
		static void CompilePixelShader(std::vector<uint8_t> v_PixelData, ShaderType type, ID3D11PixelShader** pp_Shader, GraphicsDx11* p_Gfx);

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

	private: // Buffers for loaded assets
		std::vector<ShaderDx11> mv_Shaders;
	};
}