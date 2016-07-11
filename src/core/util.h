#pragma once
#include <stdint.h>
#include <string>
#include <exception>

namespace util
{
	size_t load_buffer(std::string name, uint8_t *&buff);

	class LoadException : public std::exception
	{
	public:
		LoadException(std::string r) : reason(r) {}

		std::string reason;
		virtual const char* what() const noexcept
		{
			return reason.c_str();
		}
	};
}