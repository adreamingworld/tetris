#ifndef GFX_H
#define GFX_H

#include <SDL/SDL.h>
#include "../tetris.h"

typedef struct {
	int w,h;
	SDL_Surface *screen;
	SDL_Surface *block[7];
} Gfx;

int init_gfx(Gfx *gfx, const char *title, int w, int h);
int draw_block(Gfx *gfx, char colour, int x, int y);
void gfx_flip(Gfx *gfx);
void gfx_clear(Gfx *gfx);

#endif /* GFX_H */

