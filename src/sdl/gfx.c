#include "gfx.h"

int init_gfx(Gfx *gfx, const char *title, int w, int h)
{
	SDL_Init(SDL_INIT_VIDEO);
	
	gfx->screen = SDL_SetVideoMode(w, h, 24, 0);
	SDL_WM_SetCaption(title, NULL);
	gfx->block = SDL_LoadBMP("block.bmp");
	if (!gfx->block) printf("Error!\n");
	return 0;
}
void gfx_flip(Gfx *gfx)
{
	SDL_Flip(gfx->screen);
}

void gfx_clear(Gfx *gfx) 
{
	SDL_FillRect(gfx->screen, NULL, 0);
}

int draw_block(Gfx *gfx, char colour, int x, int y)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	SDL_BlitSurface(gfx->block, NULL, gfx->screen, &rect);
	return 0;
}

