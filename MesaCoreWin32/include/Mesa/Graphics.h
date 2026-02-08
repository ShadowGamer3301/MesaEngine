#pragma once
#include "Core.h"
#include "Window.h"
#include "Exception.h"

namespace Mesa
{
	class MSAPI Graphics
	{
	public:
		Graphics();
		virtual ~Graphics();

		virtual void DrawFrame(Window* p_Window) = 0;
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
		void RenderScene();

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
	};
}