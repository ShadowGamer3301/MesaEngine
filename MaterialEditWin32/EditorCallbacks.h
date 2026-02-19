#pragma once
#include "Core.h"
#include "EditorEvent.h"

/*
	Callback called when main window is closed
*/
inline void WindowCloseCallback(Fl_Widget* p_Widget, void* data) noexcept
{
	Mesa::Event* p_Event = new Mesa::Event();

	Mesa::EventHandler::AddEventToBuffer(EditorEventType_CloseWindow, p_Event);
}

/*
	Callback called when data in model input is updated
*/
inline void ModelTextCallback(Fl_Widget* p_Widget, void* data) noexcept
{
	Fl_Input* p_Input = (Fl_Input*)p_Widget;

	TextUpdateEvent* p_Event = new TextUpdateEvent();

	p_Event->m_NewValue = p_Input->value();

	Mesa::EventHandler::AddEventToBuffer(EditorEventType_ModelTextUpdate, p_Event);
}

/*
	Callback called when model load button is pressed
*/
inline void ModelLoadCallback(Fl_Widget* p_Widget, void* data) noexcept
{
	Mesa::Event* p_Event = new Mesa::Event();

	Mesa::EventHandler::AddEventToBuffer(EditorEventType_ModelLoad, p_Event);
}

/*
	Callback called when data in color pass input is updated
*/
inline void ColorTextCallback(Fl_Widget* p_Widget, void* data) noexcept
{
	Fl_Input* p_Input = (Fl_Input*)p_Widget;

	TextUpdateEvent* p_Event = new TextUpdateEvent();

	p_Event->m_NewValue = p_Input->value();

	Mesa::EventHandler::AddEventToBuffer(EditorEventType_ColorPassUpdate, p_Event);
}

/*
	Callback called when data in specular pass input is updated
*/
inline void SpecularTextCallback(Fl_Widget* p_Widget, void* data) noexcept
{
	Fl_Input* p_Input = (Fl_Input*)p_Widget;

	TextUpdateEvent* p_Event = new TextUpdateEvent();

	p_Event->m_NewValue = p_Input->value();

	Mesa::EventHandler::AddEventToBuffer(EditorEventType_SpecularPassUpdate, p_Event);
}

/*
	Callback called when color pass load button is pressed
*/
inline void ColorLoadCallback(Fl_Widget* p_Widget, void* data) noexcept
{
	Mesa::Event* p_Event = new Mesa::Event();

	Mesa::EventHandler::AddEventToBuffer(EditorEventType_ColorPassLoad, p_Event);
}

/*
	Callback called when specular pass load button is pressed
*/
inline void SpecularLoadCallback(Fl_Widget* p_Widget, void* data) noexcept
{
	Mesa::Event* p_Event = new Mesa::Event();

	Mesa::EventHandler::AddEventToBuffer(EditorEventType_SpecularPassLoad, p_Event);
}