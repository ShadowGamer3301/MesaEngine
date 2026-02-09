#include <Mesa/Graphics.h>
#include <Mesa/ConfigUtils.h>
#include <Mesa/FileUtils.h>
#include <Mesa/LookUpUtils.h>
#include <Mesa/ConstBuffer.h>

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
        Constructor: initializes DirectX 11 rendering pipeline
    */
    GraphicsDx11::GraphicsDx11(Window* p_Window)
    {
        LOG_F(INFO, "Initializing DirectX 11 renderer...");

        // Initialize DXGI Factory to retrieve an adapter.
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

    /*
        Draws a single frame
    */
    void GraphicsDx11::DrawFrame(Window* p_Window)
    {
        RenderColorBuffer();
        mp_SwapChain->Present(0, 0);
    }

    void GraphicsDx11::SetNumberOfLayers(const uint32_t& layers)
    {
        // Validate if the requested number of layers is valid
        if (layers > 0)
            m_NumLayers = layers;
        else // If not set it to minimal valid option
            m_NumLayers = 1;
    }

    /*
        Compiles shader pack designated for forward rendering
    */
    std::map<std::string, uint32_t> GraphicsDx11::CompileForwardShaderPack(const std::string& packPath)
    {
        // Read the contents of the pack
        std::vector<uint8_t> v_PackData = FileUtils::ReadBinaryData(packPath);
        if (v_PackData.empty()) return std::map<std::string, uint32_t>();

        // Search the lookup table for files in this pack
        auto v_entries = LookUpUtils::LoadSpecificPackInfo(packPath);

        // Calculate number of files in pack
        uint32_t numFilesInPack = 0;
        memcpy(&numFilesInPack, &v_PackData[0], sizeof(uint32_t));
        
        // Validate calculated number of files
        if(numFilesInPack <= 0) return std::map<std::string, uint32_t>();
        if(numFilesInPack != v_entries.size()) return std::map<std::string, uint32_t>();

        // Calculate all starting positions for each file
        std::vector<uint64_t> v_startingPositions;

        for (int i = 0; i < numFilesInPack; i++)
        {
            uint32_t headerPos = sizeof(uint32_t) + (sizeof(uint64_t) + sizeof(uint32_t)) * i;

            uint64_t startPos = 0;
            memcpy(&startPos, &v_PackData[headerPos], sizeof(uint64_t));

            v_startingPositions.push_back(startPos-1);
        }

        // Validate that every vertex shader has its pixel shader
        if(v_entries.size() % 2 != 0) return std::map<std::string, uint32_t>();

        std::vector<std::thread> v_CompilationThreads;

        for (int i =0;i < v_entries.size(); i += 2)
        {
            // Load vertex shader data
            std::vector<uint8_t> v_VertBuffer;
            v_VertBuffer.resize(v_entries[i].m_Size);

            uint64_t startPos = v_startingPositions[v_entries[i].m_Index];
            memcpy(&v_VertBuffer[0], &v_PackData[startPos], v_entries[i].m_Size);

            // Load pixel shader data
            std::vector<uint8_t> v_PixlBuffer;
            v_PixlBuffer.resize(v_entries[i + 1].m_Size);

            startPos = v_startingPositions[v_entries[i+1].m_Index];
            memcpy(&v_PixlBuffer[0], &v_PackData[startPos], v_entries[i+1].m_Size);

            // Begin compiling shaders on another thread
            v_CompilationThreads.push_back(std::thread(GraphicsDx11::CompileShader, v_VertBuffer, v_PixlBuffer, ShaderType_Forward, this, v_entries[i].m_OriginalName, v_entries[i+1].m_OriginalName));
        }

        // Join all compilation threads
        for (auto& compThread : v_CompilationThreads)
        {
            compThread.join();
        }

        std::map<std::string, uint32_t> result;

        // Associate shaders id with their names
        for (int i = 0; i < v_entries.size(); i += 2)
        {
            result[v_entries[i].m_OriginalName] = GetShaderIdByVertexName(v_entries[i].m_OriginalName);
        }

        return result;
    }

    /*
       Compiles shader pack designated for deferred rendering
    */
    std::map<std::string, uint32_t> GraphicsDx11::CompileDeferredShaderPack(const std::string& packPath)
    {
        // Read the contents of the pack
        std::vector<uint8_t> v_PackData = FileUtils::ReadBinaryData(packPath);
        if (v_PackData.empty()) return std::map<std::string, uint32_t>();

        // Search the lookup table for files in this pack
        auto v_entries = LookUpUtils::LoadSpecificPackInfo(packPath);

        // Calculate number of files in pack
        uint32_t numFilesInPack = 0;
        memcpy(&numFilesInPack, &v_PackData[0], sizeof(uint32_t));

        // Validate calculated number of files
        if (numFilesInPack <= 0) return std::map<std::string, uint32_t>();
        if (numFilesInPack != v_entries.size()) return std::map<std::string, uint32_t>();

        // Calculate all starting positions for each file
        std::vector<uint64_t> v_startingPositions;

        for (int i = 0; i < numFilesInPack; i++)
        {
            uint32_t headerPos = sizeof(uint32_t) + (sizeof(uint64_t) + sizeof(uint32_t)) * i;

            uint64_t startPos = 0;
            memcpy(&startPos, &v_PackData[headerPos], sizeof(uint64_t));

            v_startingPositions.push_back(startPos - 1);
        }

        // Validate that every vertex shader has its pixel shader
        if (v_entries.size() % 2 != 0) return std::map<std::string, uint32_t>();

        std::vector<std::thread> v_CompilationThreads;

        for (int i = 0; i < v_entries.size(); i += 2)
        {
            // Load vertex shader data
            std::vector<uint8_t> v_VertBuffer;
            v_VertBuffer.resize(v_entries[i].m_Size);

            uint64_t startPos = v_startingPositions[v_entries[i].m_Index];
            memcpy(&v_VertBuffer[0], &v_PackData[startPos], v_entries[i].m_Size);

            // Load pixel shader data
            std::vector<uint8_t> v_PixlBuffer;
            v_PixlBuffer.resize(v_entries[i + 1].m_Size);

            startPos = v_startingPositions[v_entries[i + 1].m_Index];
            memcpy(&v_PixlBuffer[0], &v_PackData[startPos], v_entries[i + 1].m_Size);

            // Begin compiling shaders on another thread
            v_CompilationThreads.push_back(std::thread(GraphicsDx11::CompileShader, v_VertBuffer, v_PixlBuffer, ShaderType_Deferred, this, v_entries[i].m_OriginalName, v_entries[i + 1].m_OriginalName));
        }

        // Join all compilation threads
        for (auto& compThread : v_CompilationThreads)
        {
            compThread.join();
        }

        std::map<std::string, uint32_t> result;

        // Associate shaders id with their names
        for (int i = 0; i < v_entries.size(); i += 2)
        {
            result[v_entries[i].m_OriginalName] = GetShaderIdByVertexName(v_entries[i].m_OriginalName);
        }

        return result;
    }

    /*
        Loads texture packs from archive
    */
    std::map<std::string, uint32_t> GraphicsDx11::LoadTexturePack(const std::string& packPath)
    {
        // Read the contents of the pack
        std::vector<uint8_t> v_PackData = FileUtils::ReadBinaryData(packPath);
        if (v_PackData.empty()) return std::map<std::string, uint32_t>();

        // Search the lookup table for files in this pack
        auto v_entries = LookUpUtils::LoadSpecificPackInfo(packPath);

        // Calculate number of files in pack
        uint32_t numFilesInPack = 0;
        memcpy(&numFilesInPack, &v_PackData[0], sizeof(uint32_t));

        // Validate calculated number of files
        if (numFilesInPack <= 0) return std::map<std::string, uint32_t>();
        if (numFilesInPack != v_entries.size()) return std::map<std::string, uint32_t>();

        // Calculate all starting positions for each file
        std::vector<uint64_t> v_startingPositions;

        for (int i = 0; i < numFilesInPack; i++)
        {
            uint32_t headerPos = sizeof(uint32_t) + (sizeof(uint64_t) + sizeof(uint32_t)) * i;

            uint64_t startPos = 0;
            memcpy(&startPos, &v_PackData[headerPos], sizeof(uint64_t));

            v_startingPositions.push_back(startPos - 1);
        }

        std::vector<std::thread> v_LoadThreads;

        for (int i = 0; i < v_entries.size(); i ++)
        {
            // Load texture data
            std::vector<uint8_t> v_DataBuffer;
            v_DataBuffer.resize(v_entries[i].m_Size);

            uint64_t startPos = v_startingPositions[v_entries[i].m_Index];
            memcpy(&v_DataBuffer[0], &v_PackData[startPos], v_entries[i].m_Size);

            // Begin decoding texture on another thread
            v_LoadThreads.push_back(std::thread(GraphicsDx11::LoadTexture, v_DataBuffer, this, v_entries[i].m_OriginalName));
        }

        // Join all compilation threads
        for (auto& ldThread : v_LoadThreads)
        {
            ldThread.join();
        }

        std::map<std::string, uint32_t> result;

        // Associate texture ids with their names
        for (int i = 0; i < v_entries.size(); i++)
        {
            result[v_entries[i].m_OriginalName] = GetShaderIdByVertexName(v_entries[i].m_OriginalName);
        }

        return result;
    }

    std::map<std::string, uint32_t> GraphicsDx11::LoadModelPack(const std::string& packPath)
    {
        // Read the contents of the pack
        std::vector<uint8_t> v_PackData = FileUtils::ReadBinaryData(packPath);
        if (v_PackData.empty()) return std::map<std::string, uint32_t>();

        // Search the lookup table for files in this pack
        auto v_entries = LookUpUtils::LoadSpecificPackInfo(packPath);

        // Calculate number of files in pack
        uint32_t numFilesInPack = 0;
        memcpy(&numFilesInPack, &v_PackData[0], sizeof(uint32_t));

        // Validate calculated number of files
        if (numFilesInPack <= 0) return std::map<std::string, uint32_t>();
        if (numFilesInPack != v_entries.size()) return std::map<std::string, uint32_t>();

        // Calculate all starting positions for each file
        std::vector<uint64_t> v_startingPositions;

        for (int i = 0; i < numFilesInPack; i++)
        {
            uint32_t headerPos = sizeof(uint32_t) + (sizeof(uint64_t) + sizeof(uint32_t)) * i;

            uint64_t startPos = 0;
            memcpy(&startPos, &v_PackData[headerPos], sizeof(uint64_t));

            v_startingPositions.push_back(startPos - 1);
        }

        std::vector<std::thread> v_LoadThreads;

        for (int i = 0; i < v_entries.size(); i++)
        {
            // Load texture data
            std::vector<uint8_t> v_DataBuffer;
            v_DataBuffer.resize(v_entries[i].m_Size);

            uint64_t startPos = v_startingPositions[v_entries[i].m_Index];
            memcpy(&v_DataBuffer[0], &v_PackData[startPos], v_entries[i].m_Size);

            // Begin decoding texture on another thread
            v_LoadThreads.push_back(std::thread(GraphicsDx11::LoadModel, v_DataBuffer, this, v_entries[i].m_OriginalName));
        }

        // Join all compilation threads
        for (auto& ldThread : v_LoadThreads)
        {
            ldThread.join();
        }

        std::map<std::string, uint32_t> result;

        // Associate texture ids with their names
        for (int i = 0; i < v_entries.size(); i++)
        {
            result[v_entries[i].m_OriginalName] = GetShaderIdByVertexName(v_entries[i].m_OriginalName);
        }

        return result;
    }

    /*
        Returns shader ID if the its vertex name matches with provided string
    */
    uint32_t GraphicsDx11::GetShaderIdByVertexName(const std::string& name)
    {
        for (const auto& shader : mv_Shaders)
        {
            if (strcmp(shader.GetVertexShaderName().c_str(), name.c_str()) == 0)
                return shader.GetShaderUID();
        }

        // If shader ID can't be found return 0 to indicate that the shader isn't loaded
        return 0;
    }

    /*
        Returns shader ID if the its pixel name matches with provided string
    */
    uint32_t GraphicsDx11::GetShaderIdByPixelName(const std::string& name)
    {
        for (const auto& shader : mv_Shaders)
        {
            if (strcmp(shader.GetPixelShaderName().c_str(), name.c_str()) == 0)
                return shader.GetShaderUID();
        }

        // If shader ID can't be found return 0 to indicate that the shader isn't loaded
        return 0;
    }

    /*
        Returns texture ID if the its name matches with provided string
    */
    uint32_t GraphicsDx11::GetTextureIdByName(const std::string& name)
    {
        for (const auto& shader : mv_Shaders)
        {
            if (strcmp(shader.GetPixelShaderName().c_str(), name.c_str()) == 0)
                return shader.GetShaderUID();
        }

        // If texture ID can't be found return 0 to indicate that the texture isn't loaded
        return 0;
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

    /*
        Processes nodes of the model imported by ASSIMP
    */
    void GraphicsDx11::ProcessNode(ModelDx11& outMdl, aiNode* p_Node, const aiScene* p_Scene)
    {
        // Go and process each mesh in a node
        for (size_t i = 0; i < p_Node->mNumMeshes; i++)
        {
            outMdl.mv_Meshes.push_back(ProcessMesh(p_Scene->mMeshes[p_Node->mMeshes[i]], p_Scene));
        }

        // Repeat processing for all child nodes
        for (size_t i = 0; i < p_Node->mNumChildren; i++)
        {
            ProcessNode(outMdl, p_Node->mChildren[i], p_Scene);
        }
    }

    /*
        Processes mesh data and creates vertex and index buffer for it.
    */
    MeshDx11 GraphicsDx11::ProcessMesh(aiMesh* p_Mesh, const aiScene* p_Scene)
    {
        MeshDx11 mesh;

        std::vector<VertexDx11> v_vertices;
        std::vector<uint32_t> v_indices;

        // Go through each vertex in a model and copy important data to vectors
        for (size_t i = 0; i < p_Mesh->mNumVertices; i++)
        {
            VertexDx11 vert;

            // Vertex position
            vert.m_Position.x = p_Mesh->mVertices[i].x;
            vert.m_Position.y = p_Mesh->mVertices[i].y;
            vert.m_Position.z = p_Mesh->mVertices[i].z;

            // Vertex normals
            if (p_Mesh->HasNormals())
            {
                vert.m_Normal.x = p_Mesh->mNormals[i].x;
                vert.m_Normal.y = p_Mesh->mNormals[i].y;
                vert.m_Normal.z = p_Mesh->mNormals[i].z;
            }
            else
            {
                vert.m_Normal.x = 0;
                vert.m_Normal.y = 0;
                vert.m_Normal.z = 0;
            }

            // Vertex UV mapping
            if (p_Mesh->HasTextureCoords(0))
            {
                vert.m_TexCoord.x = p_Mesh->mTextureCoords[0][i].x;
                vert.m_TexCoord.y = p_Mesh->mTextureCoords[0][i].y;
            }
            else
            {
                vert.m_TexCoord.x = 0;
                vert.m_TexCoord.y = 0;
            }

            v_vertices.push_back(vert);
        }

        // Go through each face of a model and copy its index data
        for (size_t i = 0; i < p_Mesh->mNumFaces; i++)
        {
            aiFace face = p_Mesh->mFaces[i];
            for (size_t j = 0; j < face.mNumIndices; j++)
                v_indices.push_back(face.mIndices[j]);
        }

        bool vertexResult, indexResult;

        // Create index and vertex buffers
        std::thread vertexThread(GraphicsDx11::CreateVertexBuffer, v_vertices, mesh.mp_VertexBuffer.GetAddressOf(), this, std::ref(vertexResult));
        std::thread indexThread(GraphicsDx11::CreateIndexBuffer, v_indices, mesh.mp_IndexBuffer.GetAddressOf(), this, std::ref(indexResult));

        vertexThread.join();
        indexThread.join();

        // Validate creation results
        if (!vertexResult || !indexResult)
        {
            LOG_F(ERROR, "Creation of one or more buffers failed!");
            return MeshDx11();
        }

        // Save number of indices
        mesh.m_NumIndices = v_indices.size();

        return mesh;
    }

    void GraphicsDx11::RenderColorBuffer()
    {
        float color[4] = { 0.0f, 0.2f, 0.6f, 1.0f };
        mp_Context->ClearRenderTargetView(mp_RenderTarget.Get(), color);
        mp_Context->ClearDepthStencilView(mp_DepthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    /*
        Compiles singular shader
    */
    void GraphicsDx11::CompileShader(std::vector<uint8_t> v_VertexData, std::vector<uint8_t> v_PixelData, ShaderType type, GraphicsDx11* p_Gfx, std::string vertexName, std::string pixelName)
    {
        // Check if shader isn't loaded already
        if (p_Gfx->GetShaderIdByVertexName(vertexName) != 0)
            return;

        // Create new shader instance
        ShaderDx11 shader = {};

        // Compile both vertex and pixel shader on separate threads
        std::thread vertexThread(GraphicsDx11::CompileVertexShader, v_VertexData, type, shader.mp_VertexShader.GetAddressOf(), shader.mp_InputLayout.GetAddressOf(), p_Gfx);
        std::thread pixelThread(GraphicsDx11::CompilePixelShader, v_PixelData, type, shader.mp_PixelShader.GetAddressOf(), p_Gfx);

        vertexThread.join();
        pixelThread.join();

        // Validate compilation results
        if (shader.mp_InputLayout.Get() == nullptr || shader.mp_VertexShader.Get() == nullptr || shader.mp_PixelShader.Get() == nullptr)
        {
            LOG_F(ERROR, "At least one of the shader submodules could not be initialized properly! ");
            return;
        }

        // Fill out shader details
        shader.m_ShaderType = type;
        shader.m_PixelShaderName = pixelName;
        shader.m_VertexShaderName = vertexName;
        p_Gfx->m_ShaderIdSemaphore.acquire();
        shader.m_ShaderUID = p_Gfx->GenerateShaderUID();
        p_Gfx->mv_Shaders.push_back(shader);
        p_Gfx->m_ShaderIdSemaphore.release();

        LOG_F(INFO, "Shader fully compiled with ID = %u", shader.GetShaderUID());

        return;
    }

    /*
        Compiles vertex shader
    */
    void GraphicsDx11::CompileVertexShader(std::vector<uint8_t> v_VertexData, ShaderType type, ID3D11VertexShader** pp_Shader, ID3D11InputLayout** pp_Layout, GraphicsDx11* p_Gfx)
    {
        // Set compilation flags
        UINT compileFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
        // If program runs in debug mode compile shader with DEBUG flag
        LOG_F(INFO, "Compiling shader with debug flag");
        compileFlag |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob* p_Code = nullptr;
        ID3DBlob* p_Error = nullptr;

        // Compile shader
        HRESULT hr = D3DCompile(v_VertexData.data(), v_VertexData.size(), nullptr, nullptr, nullptr, "main", "vs_5_0", compileFlag, 0, &p_Code, &p_Error);
        // Validate compilation results
        if (FAILED(hr))
        {
            if (p_Error) LOG_F(ERROR, "%s", (char*)p_Error->GetBufferPointer());
            
            return;
        }

        // Create vertex shader module
        hr = p_Gfx->mp_Device->CreateVertexShader(p_Code->GetBufferPointer(), p_Code->GetBufferSize(), nullptr, pp_Shader);
        // Validate creation results
        if (FAILED(hr))
        {
            LOG_F(ERROR, "CreateVertexShader function failed!");
            return;
        }

        // Create appropriate input layout
        if (type == ShaderType_Forward)
        {
            D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            // Validate creation results
            hr = p_Gfx->mp_Device->CreateInputLayout(layoutDesc, _countof(layoutDesc), p_Code->GetBufferPointer(), p_Code->GetBufferSize(), pp_Layout);
            if (FAILED(hr))
            {
                LOG_F(ERROR, "CreateInputLayout function failed for forward shader!");
                return;
            }
        }
        else
        {
            D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
               {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
               {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            // Validate creation results
            hr = p_Gfx->mp_Device->CreateInputLayout(layoutDesc, _countof(layoutDesc), p_Code->GetBufferPointer(), p_Code->GetBufferSize(), pp_Layout);
            if (FAILED(hr))
            {
                LOG_F(ERROR, "CreateInputLayout function failed for deferred shader!");
                return;
            }
        }

        LOG_F(INFO, "Compiled vertex shader!");
    }

    /*
        Compiles pixel shader
    */
    void GraphicsDx11::CompilePixelShader(std::vector<uint8_t> v_PixelData, ShaderType type, ID3D11PixelShader** pp_Shader, GraphicsDx11* p_Gfx)
    {
        // Set compilation flags
        UINT compileFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
        // If program runs in debug mode compile shader with DEBUG flag
        LOG_F(INFO, "Compiling shader with debug flag");
        compileFlag |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob* p_Code = nullptr;
        ID3DBlob* p_Error = nullptr;
        // Compile shader
        HRESULT hr = D3DCompile(v_PixelData.data(), v_PixelData.size(), nullptr, nullptr, nullptr, "main", "ps_5_0", compileFlag, 0, &p_Code, &p_Error);
        // Validate compilation results
        if (FAILED(hr))
        {
            if (p_Error) LOG_F(ERROR, "%s", (char*)p_Error->GetBufferPointer());

            return;
        }

        // Create pixel shader module
        hr = p_Gfx->mp_Device->CreatePixelShader(p_Code->GetBufferPointer(), p_Code->GetBufferSize(), nullptr, pp_Shader);
        // Validate creation results
        if (FAILED(hr))
        {
            LOG_F(ERROR, "CreatePixelShader function failed for deferred shader!");
            return;
        }

        LOG_F(INFO, "Compiled pixel shader!");
    }

    void GraphicsDx11::LoadTexture(std::vector<uint8_t> v_TextureData, GraphicsDx11* p_Gfx, std::string textureName)
    {
        // Check if the texture is already loaded
        if (p_Gfx->GetTextureIdByName(textureName) != 0)
            return;

        LOG_F(INFO, "Loading %s", textureName.c_str());

        // Create new texture instance
        TextureDx11 texture = {};

        // Create variables to store height and width of the texture
        uint32_t width, height; 

        std::vector<uint8_t> v_DecodedBuffer;
        uint32_t error = lodepng::decode(v_DecodedBuffer, width, height, v_TextureData);
        if (error) 
        {
            LOG_F(ERROR, "Failed to decode %s", textureName.c_str());
            return;
        }

        LOG_F(INFO, "Decoded %s", textureName.c_str());

        // Fill out DirectX structures for texture
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.CPUAccessFlags = 0;
        desc.Width = width;
        desc.Height = height;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.MipLevels = 1;
        desc.ArraySize = 1;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = v_DecodedBuffer.data();
        initData.SysMemPitch = width * 4;
        initData.SysMemSlicePitch = width * height * 4;

        HRESULT hr = p_Gfx->mp_Device->CreateTexture2D(&desc, &initData, texture.mp_RawData.GetAddressOf());
        if (FAILED(hr))
        {
            LOG_F(ERROR, "CreateTexture2D failed!");
            return;
        }

        // Fill out DirectX structures for resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
        srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv.Texture2D.MipLevels = 1;

        hr = p_Gfx->mp_Device->CreateShaderResourceView(texture.mp_RawData.Get(), &srv, texture.mp_ResourceView.GetAddressOf());
        if (FAILED(hr))
        {
            LOG_F(ERROR, "CreateShaderResourceView failed!");
            return;
        }

        // Fill out the rest of the texture details
        texture.m_TextureName = textureName;
        p_Gfx->m_TextureIdSemaphore.acquire();
        texture.m_TextureUID = p_Gfx->GenerateTextureUID();
        p_Gfx->mv_Textures.push_back(texture);
        p_Gfx->m_TextureIdSemaphore.release();
        LOG_F(INFO, "%s loaded with UID = %u", texture.GetTextureName().c_str(), texture.GetTextureUID());
        return;
    }

    /*
        Imports model data using ASSIMP library
    */
    void GraphicsDx11::LoadModel(std::vector<uint8_t> v_ModelData, GraphicsDx11* p_Gfx, std::string modelName)
    {
        LOG_F(INFO, "Loading %s", modelName.c_str());

        Assimp::Importer importer;

        // Read raw bytes and treat them as a contents of FBX file
        const aiScene* p_Scene = importer.ReadFileFromMemory(v_ModelData.data(), v_ModelData.size(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded, ".fbx");

        // Validate importing results
        if (p_Scene == nullptr)
        {
            LOG_F(ERROR, "Failed to import %s with error %s", modelName.c_str(), importer.GetErrorString());
            return;
        }

        ModelDx11 model = {};

        // Process nodes of the model
        p_Gfx->ProcessNode(model, p_Scene->mRootNode, p_Scene);

        bool bufResult = false;

        // Create constant buffer for MVP matrix
        GraphicsDx11::CreateEmptyBuffer(sizeof(ConstBufferDx11::MvpBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DEFAULT, 0, p_Gfx, model.mp_ConstBufferMVP.GetAddressOf(), bufResult);
    
        // Validate constant buffer creation
        if (!bufResult)
        {
            LOG_F(ERROR, "Cration of constant buffer failed!");
            return;
        }

        // Fill out the rest of the model details
        model.m_ModelName = modelName;
        p_Gfx->m_ModelIdSemaphore.acquire();
        model.m_ModelUID = p_Gfx->GenerateModelUID();
        p_Gfx->mv_Models.push_back(model);
        p_Gfx->m_ModelIdSemaphore.release();
        LOG_F(INFO, "%s loaded with UID = %u", model.GetModelName().c_str(), model.GetModelUID());
        return;
    }

    /*
        Creates specified DirectX buffer.
        If creation fails then function throw an exception.
    */
    void GraphicsDx11::CreateCriticalBuffer(size_t size, UINT bindFlag, D3D11_USAGE usage, UINT cpuAccess, GraphicsDx11* p_Gfx, ID3D11Buffer** pp_Buffer)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = size;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = bindFlag;
        desc.CPUAccessFlags = 0;

        THROW_IF_FAILED_DX(p_Gfx->mp_Device->CreateBuffer(&desc, nullptr, pp_Buffer));
    }

    /*
        Creates DirectX buffer with vertex data.
        Function returns ture if creation was sucessful or false otherwise
        via reference bool.
    */
    void GraphicsDx11::CreateVertexBuffer(std::vector<VertexDx11> v_verts, ID3D11Buffer** pp_Buffer, GraphicsDx11* p_Gfx, bool& result)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(VertexDx11) * v_verts.size();
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = v_verts.data();

        if (FAILED(p_Gfx->mp_Device->CreateBuffer(&desc, &data, pp_Buffer)))
            result = false;
        else
            result = true;

        return;
    }

    /*
        Creates DirectX buffer with index data.
        Function returns ture if creation was sucessful or false otherwise
        via reference bool.
    */
    void GraphicsDx11::CreateIndexBuffer(std::vector<uint32_t> v_inds, ID3D11Buffer** pp_Buffer, GraphicsDx11* p_Gfx, bool& result)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(uint32_t) * v_inds.size();
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = v_inds.data();

        if (FAILED(p_Gfx->mp_Device->CreateBuffer(&desc, &data, pp_Buffer)))
            result = false;
        else
            result = true;

        return;
    }

    /*
        Creates empty DirectX buffer.
        Function returns ture if creation was sucessful or false otherwise
        via reference bool.
    */
    void GraphicsDx11::CreateEmptyBuffer(size_t size, UINT bindFlag, D3D11_USAGE usage, UINT cpuAccess, GraphicsDx11* p_Gfx, ID3D11Buffer** pp_Buffer, bool& result)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = size;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = bindFlag;
        desc.CPUAccessFlags = 0;

        if (FAILED(p_Gfx->mp_Device->CreateBuffer(&desc, nullptr, pp_Buffer)))
            result = false;
        else
            result = true;

        return;
    }

    /*
        Generate unique shader ID
    */
    uint32_t GraphicsDx11::GenerateShaderUID()
    {
        // If no shaders have been loaded yet don't bother looking for free ID and simply give it 1
        if (mv_Shaders.empty()) return 1;

        // Start search with ID of 1 - (0 will be reserved for fallback shader)
        uint32_t id = 1;
        bool idFound = false;

        do {
            idFound = false;

            for (auto& shader : mv_Shaders)
            {
                // Check if ID is already in use
                if (shader.m_ShaderUID == id)
                    idFound = true;
            }

            // If ID is in use increment target ID and repeat the search
            if (idFound)
                id++;

        } while (idFound);

        // Return free ID
        return id;
    }

    /*
        Generate unique texture ID
    */
    uint32_t GraphicsDx11::GenerateTextureUID()
    {
        // If no textures have been loaded yet don't bother looking for free ID and simply give it 1
        if (mv_Textures.empty()) return 1;

        // Start search with ID of 1 - (0 will be reserved for fallback texture)
        uint32_t id = 1;
        bool idFound = false;

        do {
            idFound = false;

            for (auto& texture : mv_Textures)
            {
                // Check if ID is already in use
                if (texture.m_TextureUID == id)
                    idFound = true;
            }

            // If ID is in use increment target ID and repeat the search
            if (idFound)
                id++;

        } while (idFound);

        // Return free ID
        return id;
    }

    uint32_t GraphicsDx11::GenerateModelUID()
    {
        return 0;
    }

    uint32_t GraphicsDx11::GenerateMaterialUID()
    {
        return 0;
    }
}