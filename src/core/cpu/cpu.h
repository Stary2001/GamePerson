#pragma once
#include <stdint.h>
#include <stddef.h>
#include <exception>

#include "screen.h"

class CPU
{
public:
	CPU();

	bool step();
	
	uint8_t read8(uint16_t virt);
	void write8(uint16_t virt, uint8_t v);
	uint16_t read16(uint16_t virt);
	void write16(uint16_t virt, uint16_t v);

	void update_zero_flag(uint16_t r);

	uint8_t *vram; // 0x2000
	uint8_t *wram; // 0x2000
	uint8_t *hram; // 126 bytes long at ff80-fffe

	uint8_t *bios;

	uint8_t *cart;
	size_t cart_size;

	uint64_t cycles;

	GBScreen *screen;

	struct
	{
		bool bios_enabled;
	} flags;

	struct
	{
		union 
		{
			struct
			{
				uint8_t f;
				uint8_t a;
			};
			uint16_t full;
		} af;
		
		union 
		{
			struct
			{
				uint8_t c;
				uint8_t b;
			};
			uint16_t full;
		} bc;

		union 
		{
			struct
			{
				uint8_t e;
				uint8_t d;
			};
			uint16_t full;
		} de;

		union 
		{
			struct
			{
				uint8_t l;
				uint8_t h;
			};
			uint16_t full;
		} hl;

		uint16_t pc;
		uint16_t sp;
	} regs;

	enum Flag
	{
		C = (1 << 4),
		Z = (1 << 7)
	};
};

class CPUException : public std::exception
{};