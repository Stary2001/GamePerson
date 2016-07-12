#include "screen.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>

GBScreen::GBScreen(uint8_t *vram)
{
	this->vram = vram;
	fb = (uint32_t*)malloc(144 * 160 * 4); // argb8

	lcdc = 0;
	scroll_y = scroll_x = 0;
	ct = 0;
	bg_palette_reg = 0;
	sp0_palette_reg = 0;
	sp1_palette_reg = 0;

	memset(bg_palette, 0, sizeof(bg_palette));
	memset(sp0_palette, 0, sizeof(sp0_palette));
	memset(sp1_palette, 0, sizeof(sp1_palette));

	shades[3] = 0xff000000;
	shades[2] = 0xff888888;
	shades[1] = 0xffaaaaaa;
	shades[0] = 0xffffffff;

	scanline = 0;
}

GBScreen::~GBScreen()
{
	free(fb);
}

void GBScreen::refresh()
{
	memset(fb, 0xff, 160*144*4);

	if(display_enable)
	{
		if(bg_display)
		{
			uint8_t *bgtilemap = vram + (bgtile_select ? 0x1c00 : 0x1800);
			uint8_t *bgdata = vram + (tiledata_select ? 0 : 0x800);

			int x = 0;
			int y = 0;
			int i = 0;
			int j = 0;
			int k = 0;

			int bg_x = scroll_x / 8;
			int bg_y = scroll_y / 8;

			if(tiledata_select)	// 0x8000 onwards
			{
				for(y = bg_y; y < bg_y+18; y++)
				{
					for(x = bg_x; x < bg_x + 20; x++)
					{
						int idx = bgtilemap[(y%32) * 32 + (x%32)] * 16;
						for(i = 0; i < 8; i++)
						{
							for(j = 0, k = 7; j < 8; j++, k--)
							{
								int rx = (x%32)*8 + k - scroll_x;
								int ry = (y%32)*8 + i - scroll_y;
								int c = ((bgdata[idx] & (1 << j)) != 0) | ( ((bgdata[idx + 1] & (1 << j)) != 0) << 1); 
								if(c != 0)
								{
									fb[(ry%160) * 160 + rx%160] = bg_palette[c];
								}
							}
							idx += 2;
						}
					}
				}
			}
			else	// 0x8800 onwards
			{
				printf("don't currently handle high tiledata...");
			}
		}
	}
}

uint8_t GBScreen::read(uint16_t virt)
{
	uint16_t val = virt - 0xff40;
	switch(val)
	{
		case 0: // lcdc
			return lcdc;
		break;

		case 1:
			return (stat << 3) | mode | ((int)coincidence << 2);
		break;

		case 2:
			return scroll_y;
		break;

		case 3:
			return scroll_x;
		break;

		case 4:
			return scanline; // hack
		break;

		case 7:
			return bg_palette_reg;
		break;

		default:
			printf("unimplemented lcd register read %02x\n", val);
			exit(0);
		break;
	}
}

void GBScreen::build_bg_palette()
{
	bg_palette[3] = shades[(bg_palette_reg & (3 << 6)) >> 6];
	bg_palette[2] = shades[(bg_palette_reg & (3 << 4)) >> 4];
	bg_palette[1] = shades[(bg_palette_reg & (3 << 2)) >> 2];
	bg_palette[0] = shades[(bg_palette_reg & 3)];
}

void GBScreen::build_sp0_palette()
{
	sp0_palette[3] = shades[(sp0_palette_reg & (3 << 6)) >> 6];
	sp0_palette[2] = shades[(sp0_palette_reg & (3 << 4)) >> 4];
	sp0_palette[1] = shades[(sp0_palette_reg & (3 << 2)) >> 2];
}

void GBScreen::build_sp1_palette()
{
	sp1_palette[3] = shades[(sp1_palette_reg & (3 << 6)) >> 6];
	sp1_palette[2] = shades[(sp1_palette_reg & (3 << 4)) >> 4];
	sp1_palette[1] = shades[(sp1_palette_reg & (3 << 2)) >> 2];
}

void GBScreen::write(uint16_t virt, uint8_t v)
{
	uint16_t val = virt - 0xff40;

	switch(val)
	{
		case 0: // lcdc
			lcdc = v;
			display_enable = (v & (1<<7)) != 0;
			tilemap_select = (v & (1<<6)) != 0;
			window_enable = (v & (1<<5)) != 0;
			tiledata_select = (v & (1<<4)) != 0;
			bgtile_select = (v & (1<<3)) != 0;
			obj_size = (v & (1<<2)) != 0;
			obj_enable = (v & (1<<1)) != 0;
			bg_display = (v & 1) != 0;
		break;

		case 1:
			stat = v >> 3;
		break;

		case 2:
			scroll_y = v;
			refresh();
		break;

		case 3:
			scroll_x = v;
			refresh();
		break;

		case 4: // lcd line
			printf("uh\n");
			exit(0);
		break;

		case 7:
			bg_palette_reg = v;
			build_bg_palette();
		break;

		case 8:
			sp0_palette_reg = v;
			build_sp0_palette();
		break;

		case 9:
			sp1_palette_reg = v;
			build_sp1_palette();
		break;

		case 0x0a: // todo: window x / y
		case 0x0b:
		break;

		default:
			printf("unimplemented lcd register write %02x\n", val);
			exit(0);
		break;
	}
}

void GBScreen::start_frame()
{
	scanline = 0;
}

void GBScreen::step()
{
	if(scanline != VBLANK_END)
	{
		scanline++;
	}
}

void GBScreen::process_interrupts()
{
	// Stuff.
}