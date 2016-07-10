#pragma once
#include <stdint.h>
#include <string>

namespace util
{
	size_t load_buffer(std::string name, uint8_t *&buff);
}