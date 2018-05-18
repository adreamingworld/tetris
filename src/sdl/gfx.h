#ifndef GFX_H
#define GFX_H

#include <SDL.h>
#include <math.h>
#include "../tetron.h"

typedef struct {
	int w,h;
	SDL_Surface *screen;
	SDL_Surface *block[8];

	SDL_Surface *menu_stuff;
} Gfx;

int init_gfx(Gfx *gfx, const char *title, int w, int h);
int draw_block(Gfx *gfx, char colour, int x, int y);
void gfx_flip(Gfx *gfx);
void gfx_clear(Gfx *gfx);
int draw_menu(Gfx *gfx, int selected, unsigned int time);

#endif /* GFX_H */

