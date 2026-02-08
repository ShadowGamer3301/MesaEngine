#pragma once
#include "Core.h"

namespace Mesa
{
	enum EventType : uint32_t
	{
		EventType_Null = 0x000,
		EventType_KeyInput = 0x001, // Activated when key on a keyboard is pressed or released
		EventType_MouseMove = 0x002, // Activated when mouse cursor moves
		EventType_MouseButton = 0x003, // Activated when a button on a mouse is pressed or released
		EventType_GamepadConnect = 0x004, // Activated when gamepad is connected
		EventType_GamepadUpdate = 0x005, // Activated every frame (since GLFW doesnt support gamepad input callbacks)
	};

	struct Event {};

	struct KeyInputEvent : public Event
	{
		int m_Key = 0; // Key identificator (the same as in GLFW library)
		bool m_Pressed = false; // True when key is pressed or held, false when key is released
	};

	struct MouseMoveEvent : public Event
	{
		double m_X = 0.0; // Cursor position in X-axis relative to top left corner
		double m_Y = 0.0; // Cursor position in Y-axis relative to top left corner
	};

	struct MouseButtonEvent : public Event
	{
		int m_Button = 0; // Button identificator (the same as in GLFW library)
		bool m_Pressed = false; // True when key is pressed or held, false when key is released
	};

	struct GamepadConnectEvent : public Event
	{
		int m_GamepadId = 0; // Id of a gamepad
		bool m_Connected = false; // True if connects, false if disconnects
	};

	struct GamepadUpdateEvent : public Event
	{
		int m_GamepadId = 0; // Id of a gamepad
		std::array<float, 6> ma_Axes; // Value of each of the axes in gamepad (order idecntical to the GLFW library)
		std::array<bool, 15> ma_Buttons; // State of each button on the gamepad (order idecntical to the GLFW library). True if pressed false if not.
	};

	class MSAPI EventHandler
	{
	private:
		EventHandler();
		~EventHandler();

	public:
		static void AddEventToBuffer(uint32_t type, Event* p_Data);
		static void ClearEventBuffer();
		static std::map<uint32_t, Event*>& GetEventBuffer();

	private:
		std::map<uint32_t, Event*> m_EventBuffer;
		static EventHandler m_EventHandler;
	};
}