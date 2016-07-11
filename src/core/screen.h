#pragma once
#include <stdint.h>

#define VBLANK_START 144
#define VBLANK_END 153

class GBScreen
{
public:
	GBScreen(uint8_t *vram);
	GBScreen(uint8_t *vram, uint32_t *fb_data);
	~GBScreen();

	uint8_t *vram;
	uint32_t *fb;

	void refresh();
	uint8_t read(uint16_t virt);
	void write(uint16_t virt, uint8_t v);

	void start_frame();
	void step();

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

	uint8_t scanline;

private:
	void build_palette();
};