#pragma once
#include <stdint.h>

class GBScreen
{
public:
	GBScreen(uint8_t *vram);
	uint8_t *vram;
	uint32_t *fb;

	void refresh();
	uint8_t read(uint16_t virt);
	void write(uint16_t virt, uint8_t v);

	bool display_enable;
	bool tilemap_select;
	bool window_enable;
	bool tiledata_select;
	bool bgtile_select;
	bool obj_size;
	bool obj_enable;
	bool bg_display;

	uint8_t lcdc;
	uint8_t scroll_x;
	uint8_t scroll_y;
	uint8_t palette_reg;
	uint32_t palette[4];
	uint8_t ct;

	uint32_t shades[4];

private:
	void build_palette();
};