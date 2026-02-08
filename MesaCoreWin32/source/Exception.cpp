#include <Mesa/Exception.h>

namespace Mesa
{
	/*
		Constructor: Automatically captures the context of where the exception was thrown.
	*/
	Exception::Exception(std::source_location loc)
		: m_Line(loc.line()), m_File(loc.file_name()), m_Func(loc.function_name())
	{}

	/*
		Returns a descriptive string explaining the exception.
		[Overrides std::exception::what()]
	*/
	const char* Exception::what() const noexcept
	{
		// Use a string stream to format the multi-line error message.
		std::ostringstream oss;
		oss << "Exception caught!\n"
			<< "Type: " << "General Exception!\n"
			<< "File: " << m_File << "\n"
			<< "Func: " << m_Func << "\n"
			<< "Line: " << m_Line << "\n";

		// Store the formatted string in a member variable (m_WhatBuffer).
		m_WhatBuffer = oss.str();

		return m_WhatBuffer.c_str();
	}
}