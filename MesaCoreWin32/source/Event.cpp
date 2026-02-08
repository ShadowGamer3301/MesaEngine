#include <Mesa/Event.h>

namespace Mesa
{
	EventHandler EventHandler::m_EventHandler;

	/*
		Constructor
	*/
	EventHandler::EventHandler()
	{}

	/*
		Destructor.
		Releases all event pointers from the buffer and clears it.
	*/
	EventHandler::~EventHandler()
	{
		for (auto ep : m_EventBuffer)
		{
			delete ep.second;
		}
		m_EventBuffer.clear();
	}

	/*
		Adds event pointer to the buffer.
	*/
	void EventHandler::AddEventToBuffer(uint32_t type, Event* p_Data)
	{
		// If event has type NULL print warning
		if (type == 0x0000) LOG_F(WARNING, "NULL Event was sent to the buffer!");

		m_EventHandler.m_EventBuffer.emplace(std::make_pair(type, p_Data));
	}

	/*
		Releases all event pointers from the buffer and clears it.
	*/
	void EventHandler::ClearEventBuffer()
	{
		for (auto ep : m_EventHandler.m_EventBuffer)
		{
			delete ep.second;
		}
		m_EventHandler.m_EventBuffer.clear();
	}

	/*
		Returns the event buffer.
	*/
	std::map<uint32_t, Event*>& EventHandler::GetEventBuffer()
	{
		return m_EventHandler.m_EventBuffer;
	}
}