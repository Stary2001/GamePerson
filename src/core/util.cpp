#include <stdint.h>
#include <fstream>
#include "util.h"

size_t util::load_buffer(std::string name, uint8_t *&buff)
{
	std::ifstream f(name, std::ios::binary);

	f.seekg(0, std::ios::end);
	size_t s = f.tellg();
	f.seekg(0, std::ios::beg);

	buff = new uint8_t[s];

	f.read((char*)buff, s);
	f.close();

	return s;
}