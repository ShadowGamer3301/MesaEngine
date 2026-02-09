#include <Mesa/Window.h>
#include <Mesa/Event.h>

namespace Mesa
{
    /*
       Constructor: Initializes a window-specific exception.
    */
    WindowException::WindowException(int code, std::source_location loc)
        : m_Code(code),    // Store the specific error code
        Exception(loc)     // Pass the location data up to the base Exception class
    {
    }

    /*
        Returns a detailed error message including the window-specific error code.
        Overrides the base class what() to include the 'Code' field.
    */
    const char* WindowException::what() const noexcept
    {
        // String stream used to build the final diagnostic message.
        std::ostringstream oss;

        oss << "Exception caught!\n"
            << "Type: " << "Window Exception!\n" // Updated type to reflect this class
            << "File: " << m_File << "\n"
            << "Func: " << m_Func << "\n"
            << "Line: " << m_Line << "\n"
            << "Code: " << m_Code << "\n"; // Append the specific error code

        // Store the result in the buffer to ensure the pointer remains valid
        // after this function returns.
        m_WhatBuffer = oss.str();

        return m_WhatBuffer.c_str();
    }

    void Window::WindowKeyCallback(GLFWwindow* p_Window, int key, int scancode, int action, int mods)
    {
        KeyInputEvent* p_KeyEvent = new KeyInputEvent();
        p_KeyEvent->m_Key = key;
        p_KeyEvent->m_Pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);

        EventHandler::AddEventToBuffer(EventType_KeyInput, p_KeyEvent);
    }

    void Window::WindowCharCallback(GLFWwindow* p_Window, unsigned int codepoint)
    {

    }

    void Window::WindowCursorPosCallback(GLFWwindow* p_Window, double xpos, double ypos)
    {
        MouseMoveEvent* p_MouseEvent = new MouseMoveEvent();
        p_MouseEvent->m_X = xpos;
        p_MouseEvent->m_Y = ypos;

        EventHandler::AddEventToBuffer(EventType_MouseMove, p_MouseEvent);
    }

    void Window::WindowCursorEnterCallback(GLFWwindow* p_Window, int entered)
    {

    }

    void Window::WindowMouseBtnCallback(GLFWwindow* p_Window, int button, int action, int mods)
    {
        MouseButtonEvent* pMouseEvent = new MouseButtonEvent();
        pMouseEvent->m_Pressed = (action == GLFW_PRESS);
        pMouseEvent->m_Button = button;

        EventHandler::AddEventToBuffer(EventType_MouseButton, pMouseEvent);
    }

    void Window::WindowScrollCallback(GLFWwindow* p_Window, double xoffset, double yoffset)
    {
    }

    void Window::GamepadCallback(int jid, int event)
    {
        GamepadConnectEvent* p_GamepadEvent = new GamepadConnectEvent();
        p_GamepadEvent->m_Connected = (event == GLFW_CONNECTED);
        p_GamepadEvent->m_GamepadId = jid;

        EventHandler::AddEventToBuffer(EventType_GamepadConnect, p_GamepadEvent);
    }

    /*
        Constructor: Initializes the GLFW library and creates the system window.
    */
    Window::Window(uint32_t width, uint32_t height, const char* title, bool fullscreen)
    {
        // Initialize the GLFW library. Must be called before any other GLFW functions.
        if (!glfwInit())
            throw WindowException(glfwGetError(nullptr));

        // Specifically tells GLFW NOT to create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // Disallow manual window resizing by the user
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        if (fullscreen)
        {
            // Creates a window on the primary monitor for fullscreen mode
            mp_Window = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), nullptr);
        }
        else
        {
            // Creates a standard floating window
            mp_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        }

        // Validate that the window was successfully created
        if (!mp_Window)
            throw WindowException(glfwGetError(nullptr));

        // Save window fullscreen status
        m_Fullscreen = fullscreen;

        // Set appropriate callbacks
        glfwSetKeyCallback(mp_Window, Window::WindowKeyCallback);
        glfwSetJoystickCallback(Window::GamepadCallback);

        // Check for connected gamepads
        QueryConnectedGamepads();
    }

    /*
        Destructor: Ensures system resources are freed when the Window object is destroyed.
    */
    Window::~Window()
    {
        // Clean up the GLFW window.
        glfwDestroyWindow(mp_Window);
    }

    /*
        Processes OS events and checks if the window is still active.
        Returns true if the window should stay open, false if the user requested a close.
    */
    bool Window::Update()
    {
        // Process all pending events (keyboard, mouse, window movement).
        glfwPollEvents();

        // Update status of all connected gamepads
        UpdateGamepadStatus();

        // Return true as long as the close signal hasn't been sent.
        return !glfwWindowShouldClose(mp_Window);
    }

    /*
       Retrieves the raw Win32 window handle.
    */
    HWND Window::GetNativeWindow()
    {
        return glfwGetWin32Window(mp_Window);
    }

    /*
        Fetches the current width of the window's content area.
    */
    uint32_t Window::GetWindowWidth() noexcept
    {
        int width = 0;
        glfwGetWindowSize(mp_Window, &width, nullptr);
        return static_cast<uint32_t>(width);
    }

    /*
        Fetches the current height of the window's content area.
    */
    uint32_t Window::GetWindowHeight() noexcept
    {
        int height = 0;
        glfwGetWindowSize(mp_Window, nullptr, &height);
        return static_cast<uint32_t>(height);
    }

    void Window::SetGamepadStatus(const int& gpId, const bool& connected)
    {
        m_GamepadsMap[gpId] = connected;
    }

    void Window::QueryConnectedGamepads()
    {
        for (int i = 0; i < 16; i++)
        {
            if (glfwJoystickPresent(i) == GLFW_TRUE)
            {
                LOG_F(INFO, "Detected gamepad: %s", glfwGetJoystickName(i));
                m_GamepadsMap[i] = true;
            }
        }
    }

    void Window::UpdateGamepadStatus()
    {
        for (auto& gamepad : m_GamepadsMap)
        {
            if (gamepad.second)
            {
                GLFWgamepadstate state;

                if (glfwGetGamepadState(gamepad.first, &state))
                {
                    GamepadUpdateEvent* p_UpdateEvent = new GamepadUpdateEvent();

                    p_UpdateEvent->m_GamepadId = gamepad.first;

                    p_UpdateEvent->ma_Axes[GLFW_GAMEPAD_AXIS_LEFT_X] = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
                    p_UpdateEvent->ma_Axes[GLFW_GAMEPAD_AXIS_LEFT_Y] = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
                    p_UpdateEvent->ma_Axes[GLFW_GAMEPAD_AXIS_RIGHT_X] = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
                    p_UpdateEvent->ma_Axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
                    p_UpdateEvent->ma_Axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
                    p_UpdateEvent->ma_Axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];

                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_A] = state.buttons[GLFW_GAMEPAD_BUTTON_A];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_B] = state.buttons[GLFW_GAMEPAD_BUTTON_B];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_X] = state.buttons[GLFW_GAMEPAD_BUTTON_X];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_Y] = state.buttons[GLFW_GAMEPAD_BUTTON_Y];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_BACK] = state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_START] = state.buttons[GLFW_GAMEPAD_BUTTON_START];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_GUIDE] = state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
                    p_UpdateEvent->ma_Buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];

                    EventHandler::AddEventToBuffer(EventType_GamepadUpdate, p_UpdateEvent);
                }
            }
        }

    }
}