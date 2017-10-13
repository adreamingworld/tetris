#include "gfx.h"

float colours[7][3] = {
					{0.9, 0.1, 0.1},
					{0.1, 0.9, 0.4},
					{0.5, 0.4, 0.9},
					{0.7, 0.9, 0.2},
					{0.2, 0.8, 0.8},
					{0.5, 0.4, 0.5},
					{0.8, 0.8, 0.7}
					};

void colorize(SDL_Surface *s, float r, float g, float b)
{
	int i;
	unsigned char *ptr = s->pixels;
	int pitch = s->w;
	int h = s->h;

	for (i=0; i<pitch*h; i++) {
		ptr[i*3+0] = (ptr[i*3+0] * b);
		ptr[i*3+1] = (ptr[i*3+1] * g);
		ptr[i*3+2] = (ptr[i*3+2] * r);
	}
}

int init_gfx(Gfx *gfx, const char *title, int w, int h)
{
	SDL_Init(SDL_INIT_VIDEO);

	gfx->w = w;
	gfx->h = h;
	gfx->screen = SDL_SetVideoMode(w, h, 24, 0);
	SDL_WM_SetCaption(title, NULL);

	int i;
	for (i=0; i<7; i++) {
		gfx->block[i] = 0;
		gfx->block[i] = SDL_LoadBMP(DIR"block.bmp");
		if (gfx->block[i] == 0) {
			gfx->block[i] = SDL_LoadBMP("block.bmp");
		}
		colorize(gfx->block[i], colours[i][0], colours[i][1], colours[i][2]);
	}
	return 0;
}
void gfx_flip(Gfx *gfx)
{
	SDL_Flip(gfx->screen);
}

void gfx_clear(Gfx *gfx) 
{
	SDL_FillRect(gfx->screen, NULL, 0x000000);
}

int draw_block(Gfx *gfx, char colour, int x, int y)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	SDL_BlitSurface(gfx->block[colour], NULL, gfx->screen, &rect);
	return 0;
}

