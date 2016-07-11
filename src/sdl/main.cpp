#include <thread>
#include <SDL2/SDL.h>
#include "cpu.h"

int main(int argc, char ** argv)
{
	CPU *c = new CPU();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *win = SDL_CreateWindow("GamePerson", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, 0);
	SDL_Renderer *sdlRenderer = SDL_CreateRenderer(win, -1, 0);

	SDL_Texture *screen_tex = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
	int i = 0;

	bool run = true;

	SDL_Event ev;

	while(run)
	{
		c->screen->start_frame();

		for(i = 0; i < 10000; i++)
		{
			if(i % 10000/VBLANK_END == 0)
			{
				c->screen->step();
			}
			if(c->step())
			{
				run = false;
				break;
			}
		}

		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT)
			{
				break;
			}
		}

		SDL_UpdateTexture(screen_tex, NULL, c->screen->fb, 160 * sizeof (Uint32));
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, screen_tex, NULL, NULL);
		SDL_RenderPresent(sdlRenderer);
	}

	SDL_DestroyTexture(screen_tex);
	SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyWindow(win);

	delete c;

	SDL_Quit();
}