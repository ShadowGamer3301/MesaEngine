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
		int m_Key = 0;
		bool m_Pressed = false;
	};

	struct MouseMoveEvent : public Event
	{
		double m_X = 0.0;
		double m_Y = 0.0;
	};

	struct MouseButtonEvent : public Event
	{
		int m_Button = 0;
		bool m_Pressed = false;
	};

	struct GamepadConnectEvent : public Event
	{
		int m_GamepadId = 0;
		bool m_Connected = false;
	};

	struct GamepadUpdateEvent : public Event
	{
		int m_GamepadId = 0;
		std::array<float, 6> ma_Axes;
		std::array<bool, 15> ma_Buttons;
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