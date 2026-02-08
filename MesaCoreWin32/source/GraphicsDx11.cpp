#include <Mesa/Graphics.h>
#include <Mesa/ConfigUtils.h>
#include <Mesa/FileUtils.h>

namespace Mesa
{
    /*
       Constructor: Initializes a DirectX exception.
    */
	GraphicsDx11Exception::GraphicsDx11Exception(HRESULT hr, std::source_location loc)
		: m_Code(hr), Exception(loc)
	{}

    /*
       Returns a detailed error message including the window-specific error code.
       Overrides the base class what() to include the 'Code' field.
   */
	const char* GraphicsDx11Exception::what() const noexcept
	{
        // String stream used to build the final diagnostic message.
        std::ostringstream oss;

        oss << "Exception caught!\n"
            << "Type: " << "DirectX 11 Exception!\n" // Updated type to reflect this class
            << "File: " << m_File << "\n"
            << "Func: " << m_Func << "\n"
            << "Line: " << m_Line << "\n"
            << "Code: 0x" << std::hex << m_Code << std::dec << "\n"; // Append the specific error code as hexadecimal value

        // Store the result in the buffer to ensure the pointer remains valid
        // after this function returns.
        m_WhatBuffer = oss.str();

        return m_WhatBuffer.c_str();
	}

    /*
        
    */
    GraphicsDx11::GraphicsDx11(Window* p_Window)
    {
        LOG_F(INFO, "Initializing DirectX 11 renderer...");

        // Establish the DXGI Factory to interface with the OS graphics infrastructure.
        InitializeFactory();
        LOG_F(INFO, "DXGI Factory initialized");

        // Identify the physical hardware (GPU) to be used for rendering.
        mp_Adapter = FindSuitableAdapter();
        // Create the logical Device (resource creator) and Context (command issuer).
        InitializeDevice();
        LOG_F(INFO, "Device and device context initialized");

        // Create the Swap Chain to manage front/back buffers for the given window.
        InitializeSwapChain(p_Window->GetNativeWindow(), p_Window->GetWindowWidth(), p_Window->GetWindowHeight());
        LOG_F(INFO, "SwapChain initialized");
        // Set up Depth/Stencil infrastructure (Z-Buffer).
        InitializeDepthBuffer(p_Window->GetWindowWidth(), p_Window->GetWindowHeight());
        LOG_F(INFO, "Depth/stencil buffer initialized");
        InitializeDepthState();
        LOG_F(INFO, "Depth/stencil state initialized");
        InitializeDepthView();
        LOG_F(INFO, "Depth/stencil view initialized ");
        // Initialize the Render Target View (RTV).
        InitializeRenderTargetView();
        LOG_F(INFO, "Render target view initialized ");
        // Define the Viewport to map 3D clip space to the 2D window pixel area.
        InitializeViewport(p_Window->GetWindowWidth(), p_Window->GetWindowHeight());
        LOG_F(INFO, "Viewport initialized ");
        // Configure the Rasterizer (Culling, Fill Mode, etc.).
        InitializeRasterizer();
        LOG_F(INFO, "Rasterizer initialized");
        // Configure the Sampler
        InitializeSampler();
        LOG_F(INFO, "Sampler initialized");
    }

    GraphicsDx11::~GraphicsDx11()
    {
    }

    void GraphicsDx11::DrawFrame(Window* p_Window)
    {
        RenderScene();
        mp_SwapChain->Present(0, 0);
    }

    std::vector<uint32_t> GraphicsDx11::CompileShaderPack(const std::string& packPath)
    {
        
        std::vector<uint8_t> v_PackData = FileUtils::ReadBinaryData(packPath);
        if (v_PackData.empty()) return std::vector<uint32_t>();

        uint32_t numFilesInPack = 0;
        memcpy(&numFilesInPack, &v_PackData[0], sizeof(uint32_t));
        
        if(numFilesInPack <= 0) return std::vector<uint32_t>();


    }

    /*
        Creates the DXGI Factory
    */
    void GraphicsDx11::InitializeFactory()
    {
        THROW_IF_FAILED_DX(CreateDXGIFactory(IID_PPV_ARGS(mp_Factory.GetAddressOf())));
    }

    /*
        Iterates through all available GPUs on the system to find one
        capable of hardware rendering.
        Returns a pointer to the first suitable physical GPU (Adapter) found
    */
    IDXGIAdapter* GraphicsDx11::FindSuitableAdapter()
    {
        LOG_F(INFO, "Searching for suitable GPU...");

        IDXGIAdapter* p_TempAdapter = nullptr;
        UINT i = 0;

        // Loop through all available video adapters until DXGI_ERROR_NOT_FOUND is returned
        while (mp_Factory->EnumAdapters(i, &p_TempAdapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC desc = {};
            p_TempAdapter->GetDesc(&desc);

            // If the adapter has dedicated video memory, it's likely a 
            // discrete or high-quality integrated GPU, rather than a software renderer
            if (desc.DedicatedVideoMemory > 0)
            {
                LOG_F(INFO, "Suitable GPU found: %ls", desc.Description);
                return p_TempAdapter;
            }

            // Clean up the temporary pointer if this adapter doesn't meet our criteria
            p_TempAdapter->Release();
            i++;
        }

        // If no hardware adapters are found, throw an exception
        THROW_IF_FAILED_DX(DXGI_ERROR_NOT_FOUND);
    }

    /*
        Initializes the virtual device and the device context
    */
    void GraphicsDx11::InitializeDevice()
    {
        UINT devFlags = 0;

        // In debug builds, enable the D3D11 debug layer to catch API usage errors
#ifdef _DEBUG
        LOG_F(INFO, "Creating D3D11 Device with DEBUG flag");
        devFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

        // Define the versions of DirectX the application is willing to support
        D3D_FEATURE_LEVEL fLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        // Create the logical device
        THROW_IF_FAILED_DX(D3D11CreateDevice(mp_Adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, devFlags, fLevels, _countof(fLevels), D3D11_SDK_VERSION, mp_Device.GetAddressOf(), nullptr, mp_Context.GetAddressOf()));
    }


    void GraphicsDx11::InitializeSwapChain(HWND hWnd, uint32_t width, uint32_t height)
    {
        // Zero-initialize the Swap Chain description structure
        DXGI_SWAP_CHAIN_DESC sdc = {};
        sdc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Set the pixel format to 8-bit Unsigned Normalized RGBA (standard for most monitors)
        sdc.BufferDesc.Width = width; // Set buffer dimensions to match the current window size
        sdc.BufferDesc.Height = height;
        sdc.BufferCount = 1; // Use one back buffer (Double Buffering)
        sdc.Windowed = true; // Force application to run in windowed mode
        sdc.OutputWindow = hWnd; // Link the swap chain to the actual Win32 window handle (HWND)
        sdc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;// Let the display driver decide the most efficient way to present the buffer
        sdc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Identify this buffer as a Render Target so the GPU can draw into it
        sdc.SampleDesc.Count = 1; // Don't use multisampled anitaliasing
        sdc.SampleDesc.Quality = 0;

        // Call the DXGI Factory to create the actual Swap Chain object
        THROW_IF_FAILED_DX(mp_Factory->CreateSwapChain(mp_Device.Get(), &sdc, mp_SwapChain.GetAddressOf()));
    }

    /*
        Creates the Depth-Stencil texture resource
        This texture acts as the "Z-buffer" to keep track of pixel depth during rendering
    */
    void GraphicsDx11::InitializeDepthBuffer(uint32_t width, uint32_t height)
    {
        // Define the properties of the 2D texture that will hold depth data.
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1; // Depth buffers do not need mipmaps.
        desc.ArraySize = 1; // Single texture (not a texture array).
        desc.Format = DXGI_FORMAT_R24G8_TYPELESS; // Using R24G8_TYPELESS allows the texture to be interpreted differently depending on the View
        desc.Usage = D3D11_USAGE_DEFAULT; // The GPU will have both read and write access.

        // Dual Binding:
        // 1. D3D11_BIND_DEPTH_STENCIL: Allows it to be used for depth testing.
        // 2. D3D11_BIND_SHADER_RESOURCE: Allows shaders to read this buffer (e.g., for Post-Processing).
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0; // The CPU does not need to read this texture directly.
        desc.MiscFlags = 0;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        // Create the texture resource on the GPU.
        // 'nullptr' for initial data because the buffer is cleared at the start of every frame.
        THROW_IF_FAILED_DX(mp_Device->CreateTexture2D(&desc, nullptr, mp_DepthBuffer.GetAddressOf()));
    }

    /*
        Configures the Depth-Stencil State, which defines the rules for depth testing
        and stencil testing
    */
    void GraphicsDx11::InitializeDepthState()
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        // Depth Settings
        desc.DepthEnable = true; // Enable depth testing
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Allow pixels to write to the depth buffer
        desc.DepthFunc = D3D11_COMPARISON_LESS; // Standard: Pass if the new pixel is closer than the existing one
        // Stencil Settings
        desc.StencilEnable = true; // Enable the stencil buffer for masking/special effects
        desc.StencilReadMask = 0xFF; // Allow reading all 8 bits of the stencil value
        desc.StencilWriteMask = 0xFF; // Allow writing to all 8 bits
        // Front-Facing Triangle Stencil Logic
        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // Keep value if stencil test fails
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR; // Increment value if stencil passes but depth fails
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; // Keep value if both tests pass
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // Always pass the stencil test (common for simple depth setups)
        // Back-Facing Triangle Stencil Logic
        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR; // Decrement value if stencil passes but depth fails
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Create the immutable state object on the GPU
        THROW_IF_FAILED_DX(mp_Device->CreateDepthStencilState(&desc, mp_DepthState.GetAddressOf()));

        // Bind this state to the Output Merger (OM) stage of the pipeline
        mp_Context->OMSetDepthStencilState(mp_DepthState.Get(), 0);
    }

    /*
        Creates the Depth Stencil View (DSV) from the depth buffer resource.
        This view allows the Output Merger stage to perform depth and stencil testing.
    */
    void GraphicsDx11::InitializeDepthView()
    {
        // Define how the depth-stencil resource should be accessed.
        D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Specify the specific data format. D24 = 24 bits for Depth, S8 = 8 bits for Stencil
        desc.Flags = 0;
        // For standard rendering, use the basic Texture2D dimension.
        desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        // Use the first (and only) mipmap level for depth testing.
        desc.Texture2D.MipSlice = 0;
        // Create the View on the device using the mp_DepthBuffer resource.
        THROW_IF_FAILED_DX(mp_Device->CreateDepthStencilView(mp_DepthBuffer.Get(), &desc, mp_DepthView.GetAddressOf()));
    }

    /*
        Creates the Render Target View (RTV) from the Swap Chain's back buffer
        This links the output of the rendering pipeline to the window's display buffer
    */
    void GraphicsDx11::InitializeRenderTargetView()
    {
        // Retrieve a pointer to the back buffer (index 0) from the Swap Chain.
        THROW_IF_FAILED_DX(mp_SwapChain->GetBuffer(0, IID_PPV_ARGS(mp_BackBuffer.GetAddressOf())));

        // Create the Render Target View using the back buffer resource
        // Passing 'nullptr' for the description defaults to a view that matches 
        // the back buffer's existing format (e.g., R8G8B8A8_UNORM)
        THROW_IF_FAILED_DX(mp_Device->CreateRenderTargetView(mp_BackBuffer.Get(), nullptr, mp_RenderTarget.GetAddressOf()));

        // Bind the Render Target and the Depth/Stencil view to the Output Merger (OM) stage
        mp_Context->OMSetRenderTargets(1, mp_RenderTarget.GetAddressOf(), mp_DepthView.Get());
    }

    /*
        Configures the Viewport, which maps the normalized clip space coordinates
        to the actual pixel coordinates of the window
    */
    void GraphicsDx11::InitializeViewport(uint32_t width, uint32_t height)
    {
        // Define the dimensions and depth range of the render area
        D3D11_VIEWPORT vp = {};
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        // Set the viewport size to match the window's current width and height
        vp.Width = static_cast<float>(width);
        vp.Height = static_cast<float>(height);
        // Define the depth range
        vp.MaxDepth = 1.0f;
        vp.MinDepth = 0.0f;
        // Bind the viewport to the Rasterizer Stage (RS) of the pipeline
        mp_Context->RSSetViewports(1, &vp);
    }

    /*
        Configures the Rasterizer State, which determines how triangles are
        rasterized by the GPU
    */
    void GraphicsDx11::InitializeRasterizer()
    {
        // Define the description for the rasterizer state.
        D3D11_RASTERIZER_DESC dsc = {};
        // D3D11_FILL_SOLID: Draw solid triangles.
        dsc.FillMode = D3D11_FILL_SOLID;
        // D3D11_CULL_BACK: Do not draw the back-facing triangles.
        dsc.CullMode = D3D11_CULL_BACK;
        // Defines "Front" as triangles with vertices in clockwise order.
        dsc.FrontCounterClockwise = false;
        // Depth Bias settings (used primarily to prevent "shadow acne" in shadow mapping).
        // Set to 0/0.0f as standard defaults for general rendering.
        dsc.DepthBias = 0;
        dsc.DepthBiasClamp = 0.0f;
        dsc.SlopeScaledDepthBias = 0.0f;
        // Determines if pixels outside the near/far clipping planes are discarded.
        dsc.DepthClipEnable = false;
        // Scissor testing allows you to discard pixels outside a specific rectangular area.
        dsc.ScissorEnable = false;
        // Disable support for multisampled anti-aliasing
        dsc.MultisampleEnable = false; 
        dsc.AntialiasedLineEnable = false;
        // Create the immutable state object on the GPU device.
        THROW_IF_FAILED_DX(mp_Device->CreateRasterizerState(&dsc, mp_RasterizerState.GetAddressOf()));

        // Bind the state to the Rasterizer Stage (RS) of the pipeline.
        mp_Context->RSSetState(mp_RasterizerState.Get());
    }

    /*
        Initializes the Sampler State, which controls how textures are filtered
        and wrapped when applied to 3D geometry.
    */
    void GraphicsDx11::InitializeSampler()
    {
        // Define the properties for texture sampling.
        D3D11_SAMPLER_DESC desc = {};
        // D3D11_FILTER_MIN_MAG_MIP_LINEAR: Enables Trilinear filtering.
        // This smooths the texture when it is scaled up (Mag), scaled down (Min), 
        // and blends between different Mipmap levels to prevent shimmering.
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

        // AddressU/V/W define what happens when texture coordinates fall outside [0, 1].
        // D3D11_TEXTURE_ADDRESS_WRAP: The texture repeats (tiles) infinitely.
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Horizontal tiling
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Vertical tiling
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Depth tiling (for 3D textures)
        // ComparisonFunc is typically used for "Shadow Samplers" (Depth comparison).
        // Set to NEVER for standard color texture sampling.
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        // Define the range of the Mipmap levels to use.
        // MinLOD 0 allows the GPU to use the highest-resolution version of the texture.
        desc.MinLOD = 0;
        // MaxLOD set to max float allows the GPU to use all available mipmap levels.
        desc.MaxLOD = D3D11_FLOAT32_MAX;

        // Create the immutable sampler state object on the GPU.
        THROW_IF_FAILED_DX(mp_Device->CreateSamplerState(&desc, mp_Sampler.GetAddressOf()));
    }

    void GraphicsDx11::RenderScene()
    {
        float color[4] = { 0.0f, 0.2f, 0.6f, 1.0f };
        mp_Context->ClearRenderTargetView(mp_RenderTarget.Get(), color);
        mp_Context->ClearDepthStencilView(mp_DepthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}