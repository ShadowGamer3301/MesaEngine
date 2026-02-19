#pragma once
#include "Core.h"

enum EditorEventType : uint32_t
{
	EditorEventType_CloseWindow = 0x0001,
	EditorEventType_ModelTextUpdate = 0x0002,
	EditorEventType_ModelLoad = 0x0003,
	EditorEventType_ColorPassUpdate = 0x0004,
	EditorEventType_ColorPassLoad = 0x0005,
	EditorEventType_SpecularPassUpdate = 0x0006,
	EditorEventType_SpecularPassLoad = 0x0007,
};

struct TextUpdateEvent : public Mesa::Event
{
	std::string m_NewValue;
};