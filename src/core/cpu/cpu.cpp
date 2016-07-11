#include "cpu.h"
#include "util.h"
#include <string.h>
#include <iostream>

CPU::CPU()
{
	size_t s = util::load_buffer("gb.bios", bios);
	if(s != 256)
	{
		throw util::LoadException("BIOS has wrong size!");
	}

	flags.bios_enabled = true;

	memset(&regs, 0, sizeof(regs));

	vram = (uint8_t*)malloc(0x2000);
	wram = (uint8_t*)malloc(0x4000);
	hram = (uint8_t*)malloc(126);

	cart_size = util::load_buffer("cart.bin", cart);
	cycles = 0;

	screen = new GBScreen(vram);

	int_enable = false;
}

CPU::~CPU()
{
	free(vram);
	free(wram);
	free(hram);
	free(bios);
	free(cart);

	delete screen;
}

uint8_t CPU::read8(uint16_t virt)
{
	if(virt < 0x100 && flags.bios_enabled)
	{
		return bios[virt];
	}
	else if(virt <= cart_size)
	{
		return cart[virt];
	}
	else if(virt >= 0x8000 && virt <= 0x9fff)
	{
		return vram[virt - 0x8000];
	}
	else if(virt >= 0xc000 && virt <= 0xdfff)
	{
		return wram[virt - 0xc000];
	}
	else if(virt >= 0xe000 && virt <= 0xfdff) // wram mirror
	{
		return wram[virt - 0xe000];
	}
	else if(virt >= 0xff40 && virt <= 0xff4f) // this is the LCD!
	{
		return screen->read(virt);
	}
	else if(virt >= 0xff80 && virt <= 0xfffe)
	{
		return hram[virt - 0xff80];
	}
	else
	{
		printf("unhandled read8 at %04x\n", virt);
		exit(0);
	}
}

void CPU::write8(uint16_t virt, uint8_t v)
{
	if(virt >= 0x8000 && virt <= 0x9fff)
	{
		vram[virt - 0x8000] = v;
	}
	else if(virt >= 0xc000 && virt <= 0xdfff)
	{
		wram[virt - 0xc000] = v;
	}
	else if(virt >= 0xe000 && virt <= 0xfdff) // wram mirror
	{
		wram[virt - 0xe000] = v;
	}
	else if(virt >= 0xff80 && virt <= 0xfffe)
	{
		hram[virt - 0xff80] = v;
	}
	else if(virt >= 0xff40 && virt <= 0xff4f) // this is the LCD!
	{
		screen->write(virt, v);
	}
	else if(virt == 0xff50)
	{
		if(v == 1)
		{
			flags.bios_enabled = false;
		}
	}
	else
	{
		printf("unhandled write8 of %02x at %04x\n", v, virt);
	}
}

uint16_t CPU::read16(uint16_t virt)
{
	if(virt < 0x100  && flags.bios_enabled)
	{
		return bios[virt] | (bios[virt+1] << 8);
	}
	else if(virt <= cart_size)
	{
		return cart[virt] | (cart[virt+1] << 8);
	}
	else if(virt >= 0x8000 && virt <= 0x9fff)
	{
		return vram[virt - 0x8000] | (vram[virt - 0x8000 + 1] << 8);
	}
	else if(virt >= 0xc000 && virt <= 0xdfff)
	{
		return wram[virt - 0xc000] | (wram[virt - 0xc000 + 1] << 8);
	}
	else if(virt >= 0xe000 && virt <= 0xfdff) // wram mirror
	{
		return wram[virt - 0xe000] | (wram[virt - 0xe000 + 1] << 8);
	}
	else if(virt >= 0xff80 && virt <= 0xfffe)
	{
		return hram[virt - 0xff80] | (hram[virt - 0xff80 + 1] << 8);
	}
	else
	{
		printf("unhandled read16 at %04x\n", virt);
		exit(0);
	}
}

void CPU::write16(uint16_t virt, uint16_t v)
{
	if(virt >= 0x8000 && virt <= 0x9fff)
	{
		vram[virt - 0x8000] = v & 0xff;
		vram[virt - 0x8000 + 1] = v >> 8;
	}
	else if(virt >= 0xc000 && virt <= 0xdfff)
	{
		wram[virt - 0xc000] = v & 0xff;
		wram[virt - 0xc000 + 1] = v >> 8;
	}
	else if(virt >= 0xe000 && virt <= 0xfdff) // wram mirror
	{
		wram[virt - 0xe000] = v & 0xff;
		wram[virt - 0xe000 + 1] = v >> 8;
	}
	else if(virt >= 0xff80 && virt <= 0xfffe)
	{
		hram[virt - 0xff80] = v & 0xff;
		hram[virt - 0xff80 + 1] = v >> 8;
	}
	else
	{
		printf("unhandled write16 of %04x at %04x\n", v, virt);
	}
}

void CPU::update_zero_flag(uint16_t v)
{
	if(v == 0)
	{
		regs.af.f |= Flag::Z;
	}
	else
	{
		regs.af.f &= ~Flag::Z;
	}
}

bool CPU::step()
{
	uint8_t instr = read8(regs.pc);

	bool jump = false;

	switch(instr)
	{
		case 0:
			// nop.
		break;

		case 0x01: // ld bc, nn
			regs.bc.full = read16(regs.pc+1);
			regs.pc += 2;
			cycles += 12;
		break;

		case 0x02: // ld (bc), a
			regs.af.a = read8(regs.bc.full);
			cycles += 8;
		break;

		case 0x03: // inc bc
			regs.bc.full++;
			cycles += 8;
		break;

		case 0x04: // inc b
			regs.bc.b++;
			cycles += 4;
		break;

		case 0x05: // dec b
			regs.bc.b--;
			update_zero_flag(regs.bc.b);
			cycles += 4;
		break;

		case 0x06: // ld b, n
			regs.bc.b = read8(regs.pc+1);
			regs.pc++;
			cycles += 8;
		break;

		case 0x0c: // inc c
			regs.bc.c++;
			cycles += 4;
		break;

		case 0x0d: // dec c
			regs.bc.c--;
			update_zero_flag(regs.bc.c);
			cycles += 4;
		break;

		case 0x0e: // ld c, n
			regs.bc.c = read8(regs.pc+1);
			regs.pc++;
			cycles += 8;
		break;

		case 0x11: // ld de, nn
			regs.de.full = read16(regs.pc+1);
			regs.pc += 2;
			cycles += 12;
		break;

		case 0x13: // inc de
			regs.de.full++;
			cycles += 8;
		break;

		case 0x15: // dec d
			regs.de.d--;
			update_zero_flag(regs.de.d);
			cycles += 4;
		break;

		case 0x16: // ld d, n
			regs.de.d = read8(regs.pc+1);
			regs.pc++;
			cycles += 8;
		break;

		case 0x17: // rla
		{
			bool c = regs.af.a & 0x80;
			regs.af.a = (regs.af.f & Flag::C ? 1 : 0) | (regs.af.a << 1);
			if(c)
			{ regs.af.f |= Flag::C; }
			else
			{ regs.af.f &= ~Flag::C; }
			cycles += 4;
		}
		break;

		case 0x18: // jr n, relative jump
		{
			int8_t ofs = (int8_t)read8(regs.pc+1);
			regs.pc++;
			regs.pc += ofs;
			cycles += 12;
		}
		break;

		case 0x1a: // ld a, (de)
			regs.af.a = read8(regs.de.full);
			cycles += 8;
		break;

		case 0x1d: // dec e
			regs.de.e--;
			update_zero_flag(regs.de.e);
			cycles += 4;
		break;

		case 0x1e: // ld e, n
			regs.de.e = read8(regs.pc+1);
			regs.pc++;
			cycles += 8;
		break;


		case 0x20: // jnz, r8
		{
			int8_t ofs = (int8_t)read8(regs.pc+1);
			regs.pc++;

			if((regs.af.f & Flag::Z) == 0)
			{
				//printf("c: %02x\n", regs.bc.c);
				//printf("ofs %i\n", ofs);
				regs.pc += ofs;
				cycles += 12;
			}
			else
			{
				cycles += 8;
			}
		}
		break;

		case 0x21: // ld hl, nn
			regs.hl.full = read16(regs.pc+1);
			regs.pc+=2;
			cycles += 12;
		break;

		case 0x22: // LDI  (HL),A
			write8(regs.hl.full, regs.af.a);
			regs.hl.full++;
			cycles += 8;
		break;

		case 0x23: // inc hl
			regs.hl.full++;
			cycles += 8;
		break;

		case 0x24: // inc h
			regs.hl.h++;
			cycles += 4;
		break;

		case 0x28: // jz n, relative jump if zero
		{
			int8_t ofs = (int8_t)read8(regs.pc+1);
			regs.pc++;

			if(regs.af.f & Flag::Z)
			{
				//printf("c: %02x\n", regs.bc.c);
				//printf("ofs %i\n", ofs);
				regs.pc += ofs;
				cycles += 12;
			}
			else
			{
				cycles += 8;
			}
		}
		break;

		case 0x2e: // ld l, n
			regs.hl.l = read8(regs.pc+1);
			regs.pc++;
			cycles += 8;
		break;

		case 0x31: // ld sp, nn
			regs.sp = read16(regs.pc+1);
			regs.pc+=2;
			cycles += 12;
		break;

		case 0x32: // ldd (hl), a
			write8(regs.hl.full, regs.af.a);
			regs.hl.full--;
			cycles += 8;
		break;

		case 0x3d: // dec a
			regs.af.a--;
			update_zero_flag(regs.af.a);
			cycles += 4;
		break;

		case 0x3e: // ld a, n
			regs.af.a = read8(regs.pc+1);
			regs.pc++;
			cycles += 8;
		break;

		case 0x4f: // ld c, a
			regs.bc.c = regs.af.a;
			cycles += 4;
		break;

		case 0x57: // ld d, a
			regs.de.d = regs.af.a;
			cycles += 4;
		break;

		case 0x67: // ld h, a
			regs.hl.h = regs.af.a;
			cycles += 4;
		break;

		case 0x7b: // ld a, e
			regs.af.a = regs.de.e;
			cycles += 4;
		break;

		case 0x7c: // ld a, h
			regs.af.a = regs.hl.h;
			cycles += 4;
		break;

		case 0x77: // ld (hl), a
			write8(regs.hl.full, regs.af.a);
			cycles += 8;
		break;

		case 0x78: // ld a, b
			regs.af.a = regs.bc.b;
			cycles += 4;
		break;

		case 0x7d: // ld a, l
			regs.af.a = regs.hl.l;
			cycles += 4;
		break;

		case 0x86: // add a, (hl)
			{
				uint8_t val = read8(regs.hl.full);
				regs.af.a += val;
				//todo: overflow
				update_zero_flag(regs.af.a);
				cycles += 8;
			}
		break;

		case 0x90: // sub b
			regs.af.a -= regs.bc.b; // todo: detect overflow.
			update_zero_flag(regs.af.a);
			cycles += 4;
		break;

		case 0xaf: // xor a
			regs.af.a = 0;
			cycles += 4;
		break;

		case 0xbe: // cp (hl)
		{
			uint8_t val = read8(regs.hl.full);
			update_zero_flag(regs.af.a - val);
			cycles += 8;
		}
		break;

		case 0xc1: // pop bc
			regs.bc.full = read16(regs.sp);
			regs.sp += 2;
			cycles += 12;
		break;

		case 0xc3: // jp nn, absolute jump
			regs.pc = read16(regs.pc+1);
			jump = true;
			cycles += 12;
		break;

		case 0xc5: // push bc
			regs.sp -= 2;
			write16(regs.sp, regs.bc.full);
			cycles += 16;
		break;

		case 0xc9: // ret
			regs.pc = read16(regs.sp);
			regs.sp += 2;
			jump = true;
			cycles += 16;
		break;

		case 0xcb: // BIT OPERATIONS
		{
			uint8_t bitop = read8(regs.pc+1);
			regs.pc++;

			static uint8_t *regnums[0x8] = 
			{
				&regs.bc.b,
				&regs.bc.c,
				&regs.de.d,
				&regs.de.e,
				&regs.hl.h,
				&regs.hl.l,
				nullptr, // memory
				&regs.af.a
			};

			switch(bitop & 0xf0)
			{
				/*case 0x00:
					// rlc/rrc
					{
						uint8_t *r = regnums[(bitop&0xf) % 8];
						if((bitop & 0xf) < 0x8) // left
						{
							if(r == nullptr)
							{

							}
							else
							{
								
							}
						}
						else // right
						{
							if(r == nullptr)
							{

							}
							else
							{

							}
						}
					}
				break;*/

				case 0x10:
					{
						// rl/rr

						uint8_t *r = regnums[(bitop&0xf) % 8];
						uint8_t v;
						bool c = false;

						if(r == nullptr) { v = read8(regs.hl.full); }
						else { v = *r; }

						if((bitop & 0xf) < 0x8) // left
						{
							c = (v & 0x80) == 0x80;
							v = (regs.af.f & Flag::C ? 1 : 0) | (v << 1);
						}
						else // right
						{
							c = (v & 1) == 1;
							v = (regs.af.f & Flag::C ? 0x80 : 0) | (v >> 1);
						}
						
						if(c)
						{
							regs.af.f |= Flag::C;
						}
						else
						{
							regs.af.f &= ~Flag::C;
						}

						if(r == nullptr) { write8(regs.hl.full, v); }
						else { *r = v; }
					}
				break;

				/*case 0x20: // sla/rla
					// rl/rr
					if((bitop & 0xf) < 0x8) // left
					{
						uint8_t *r = regnums[(bitop&0xf) % 8];
						if(r == nullptr)
						{

						}
						else
						{

						}
					}
					else // right
					{
						uint8_t *r = regnums[(bitop&0xf) % 8];
						if(r == nullptr)
						{

						}
						else
						{

						}
					}
				break;


				case 0x30: // sll/srl
					if((bitop & 0xf) < 0x8) // left
					{
						uint8_t *r = regnums[(bitop&0xf) % 8];
						if(r == nullptr)
						{

						}
						else
						{

						}
					}
					else // right
					{
						uint8_t *r = regnums[(bitop&0xf) % 8];
						if(r == nullptr)
						{

						}
						else
						{

						}
					}
				break;*/

				case 0x40: // bit
				case 0x50:
				case 0x60:
				case 0x70:
				{
					uint8_t val;

					uint8_t *r = regnums[(bitop&0xf) % 8];
					if(r == nullptr)
					{
						val = read8(regs.hl.full);
					}
					else
					{
						val = *r;
					}

					uint8_t bit = (((bitop & 0xf0) / 0x10) - 4) * 2 + ((bitop&0xf)>7 ? 1 : 0);
					if(val & (1 << bit))
					{
						regs.af.f &= ~Flag::Z; // Clear the zero flag.
					}
					else
					{
						regs.af.f |= Flag::Z;
					}
				}
				break;

				case 0x80: // res
				case 0x90:
				case 0xa0:
				case 0xb0:
				break;

				case 0xc0: // set
				case 0xd0:
				case 0xe0:
				case 0xf0:
				break;

				default:
					printf("unhandled bitop %02x at pc %04x\n", bitop, regs.pc);
					return true;
				break;
			}

			cycles += 8;
		}
		break;

		case 0xcd: // call nn
		{
			regs.sp -= 2;
			write16(regs.sp, regs.pc + 3);
			regs.pc = read16(regs.pc + 1);
			jump = true;

			cycles += 24;
		}
		break;

		case 0xe0: // LD (FF00+n),A
		{
			uint8_t val = read8(regs.pc+1);
			regs.pc++;

			write8(0xff00 + val, regs.af.a);

			cycles += 12;
		}
		break;

		case 0xe2: // LD (FF00+C),A
			write8(0xff00 + regs.bc.c, regs.af.a);
			cycles += 8;
		break;

		case 0xea: // LD (nn), A
		{
			uint16_t val = read16(regs.pc+1);
			regs.pc+=2;
			write8(val, regs.af.a);
			cycles += 16;
		}
		break;

		case 0xf0: // LD A,(FF00+n)
		{
			uint8_t val = read8(regs.pc+1);
			regs.pc++;
			regs.af.a = read8(0xff00 + val);
			cycles += 12;
		}
		break;

		case 0xf3: // di - disable interrupts
			int_enable = false;
			cycles += 4;
		break;

		case 0xfe: // cp n
		{
			uint8_t v = read8(regs.pc+1);
			regs.pc++;
			update_zero_flag(regs.af.a - v);
			cycles += 8;
		}
		break;

		default:
			printf("unhandled opcode %02x at pc %04x\n", instr, regs.pc);
			return true;
		break;
	}

	if(!jump)
	{
		regs.pc++;
	}
	else
	{
		jump = false;
	}

	return false;
}