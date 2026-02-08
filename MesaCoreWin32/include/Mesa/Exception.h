#pragma once
#include "Core.h"

namespace Mesa
{
	class MSAPI Exception : public std::exception
	{
	public:
		Exception(std::source_location loc = std::source_location::current());
		const char* what() const noexcept override;

	protected:
		uint32_t m_Line;
		std::string m_Func, m_File;
		mutable std::string m_WhatBuffer;
	};
}