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
	palette_reg = 0;
	
	memset(&palette, 0, sizeof(palette));

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
									fb[(ry%160) * 160 + rx%160] = palette[c];
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
			return palette_reg;
		break;

		default:
			printf("unimplemented lcd register read %02x\n", val);
			exit(0);
		break;
	}
}

void GBScreen::build_palette()
{
	palette[3] = shades[(palette_reg & (3 << 6)) >> 6];
	palette[2] = shades[(palette_reg & (3 << 4)) >> 4];
	palette[1] = shades[(palette_reg & (3 << 2)) >> 2];
	palette[0] = shades[(palette_reg & 3)];
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
			palette_reg = v;
			build_palette();
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