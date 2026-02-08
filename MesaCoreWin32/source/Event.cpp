#include <Mesa/Event.h>

namespace Mesa
{
	EventHandler EventHandler::m_EventHandler;

	EventHandler::EventHandler()
	{
	}

	EventHandler::~EventHandler()
	{
		for (auto ep : m_EventBuffer)
		{
			delete ep.second;
		}
		m_EventBuffer.clear();
	}

	void EventHandler::AddEventToBuffer(uint32_t type, Event* p_Data)
	{
		if (type == 0x0000) LOG_F(WARNING, "NULL Event was sent to the buffer!");

		m_EventHandler.m_EventBuffer.emplace(std::make_pair(type, p_Data));
	}

	void EventHandler::ClearEventBuffer()
	{
		for (auto ep : m_EventHandler.m_EventBuffer)
		{
			delete ep.second;
		}
		m_EventHandler.m_EventBuffer.clear();
	}

	std::map<uint32_t, Event*>& EventHandler::GetEventBuffer()
	{
		return m_EventHandler.m_EventBuffer;
	}
}