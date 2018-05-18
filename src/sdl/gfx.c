#include "gfx.h"
#define SCALE 3

float colours[8][3] = {
					{0.4, 0.4, 0.4},
					{0.2, 1.0, 0.2},
					{0.1, 0.1, 1.0}, /* blue */
					{1.0, 1.0, 0.3},
					{0.3, 1.0, 1.0},
					{0.6, 0.0, 0.6},
					{1.0, 0.6, 1.0},
					{0.6, 0.3, 0.2} /* brown */
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

int scale(SDL_Surface **original, int scale)
{
	SDL_Surface *new;
	SDL_Surface *old = *original;
	int i;
	unsigned char *nptr, *optr;

	new = SDL_CreateRGBSurface(0, old->w*scale, old->h*scale, old->format->BitsPerPixel, old->format->Rmask, old->format->Gmask, old->format->Bmask, old->format->Amask);
	nptr = new->pixels;
	optr = old->pixels;

	for (i=0; i<new->w*new->h; i++) {
		int x = i%new->w;
		int y = i/new->w;
		int ox = x/scale;
		int oy = y/scale;

		nptr[(x + y*new->w )*3+0] = optr[(ox + oy*old->w)*3+0];
		nptr[(x + y*new->w )*3+1] = optr[(ox + oy*old->w)*3+1];
		nptr[(x + y*new->w )*3+2] = optr[(ox + oy*old->w)*3+2];
	}

	SDL_FreeSurface(old);
	*original = new;

	return 0;
}

int init_gfx(Gfx *gfx, const char *title, int w, int h)
{
	SDL_Init(SDL_INIT_VIDEO);

	gfx->w = w;
	gfx->h = h;
	gfx->screen = SDL_SetVideoMode(w, h, 24, 0);
	SDL_WM_SetCaption(title, NULL);

	int i;
	for (i=0; i<8; i++) {
		gfx->block[i] = 0;
		gfx->block[i] = SDL_LoadBMP(DATA_DIR"res/block.bmp");
		if (gfx->block[i] == 0) {
			gfx->block[i] = SDL_LoadBMP("res/block.bmp");
			if (gfx->block[i] == 0) {
				printf("Failed to load res/block.bmp\n");
				exit(-1);
			}
		}
		colorize(gfx->block[i], colours[i][0], colours[i][1], colours[i][2]);
		scale(&gfx->block[i], 2);
	}

	gfx->menu_stuff = SDL_LoadBMP(DATA_DIR"res/menu.bmp");
	if (gfx->menu_stuff == 0) {
		gfx->menu_stuff = SDL_LoadBMP("res/menu.bmp");
		if (gfx->menu_stuff == 0) {
			printf("Failed to load res/menu.bmp.\n");
			exit(-1);
		}
	}
	scale(&gfx->menu_stuff, SCALE);

	int r = SDL_SetColorKey(gfx->menu_stuff, SDL_SRCCOLORKEY, 0x00000000);


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

void blit_part(SDL_Surface *surface, SDL_Surface *screen, int src[6])
{
	SDL_Rect srect, drect;
	drect.x = src[0]*SCALE;
	drect.y = src[1];

	srect.x = src[2]*SCALE;
	srect.y = src[3]*SCALE;
	srect.w = src[4]*SCALE;
	srect.h = src[5]*SCALE;
	SDL_BlitSurface(surface, &srect, screen, &drect);
}

int draw_menu(Gfx *gfx, int selected, unsigned int time)
{
	int width = 16 + gfx->screen->w/SCALE;

	int toffset[] = {32,16*SCALE*5};
	int sta_pos[] = {toffset[0]+16, 	toffset[1]+16*SCALE*0, 	16,	16,	16*3, 	16};
	int con_pos[] = {toffset[0]+16, 	toffset[1]+16*SCALE*1, 	0,	16*3,	16*4, 	16};
	int qui_pos[] = {toffset[0]+16, 	toffset[1]+16*SCALE*2, 	16,	16*2,	16*3, 	16};

	int sel_pos[] = {12+ toffset[0] + (sin(time/100.0f)*2), 	toffset[1]+16*SCALE*selected, 	0,	0,	16, 	16};

	int tee_pos[] = {-16 + (16*0 + (time/23))%width, 	40+(sin((time+(200*0))/250.0f) * 16), 	16,	0,	16, 	16};
	int eee_pos[] = {-16 + (16*1 + (time/23))%width, 	40+(sin((time+(200*1))/250.0f) * 16), 	0,	16,	16, 	16};
	int te2_pos[] = {-16 + (16*2 + (time/23))%width, 	40+(sin((time+(200*2))/250.0f) * 16), 	16,	0,	16, 	16};
	int rrr_pos[] = {-16 + (16*3 + (time/23))%width, 	40+(sin((time+(200*3))/250.0f) * 16), 	0,	16*2,	16, 	16};
	int ooo_pos[] = {-16 + (16*4 + (time/23))%width, 	40+(sin((time+(200*4))/250.0f) * 16), 	16*3,	0,	16, 	16};
	int nnn_pos[] = {-16 + (16*5 + (time/23))%width, 	40+(sin((time+(200*5))/250.0f) * 16), 	16*2,	0,	16, 	16};

	SDL_Rect fillrect;
	fillrect.x = 0;
	fillrect.y = 32;
	fillrect.w = gfx->w;
	fillrect.h = 64;
	SDL_FillRect(gfx->screen, &fillrect, 0x000033);

	blit_part(gfx->menu_stuff, gfx->screen, sta_pos);
	blit_part(gfx->menu_stuff, gfx->screen, qui_pos);
	blit_part(gfx->menu_stuff, gfx->screen, con_pos);
	blit_part(gfx->menu_stuff, gfx->screen, sel_pos);

	blit_part(gfx->menu_stuff, gfx->screen, tee_pos);
	blit_part(gfx->menu_stuff, gfx->screen, eee_pos);
	blit_part(gfx->menu_stuff, gfx->screen, te2_pos);
	blit_part(gfx->menu_stuff, gfx->screen, rrr_pos);
	blit_part(gfx->menu_stuff, gfx->screen, ooo_pos);
	blit_part(gfx->menu_stuff, gfx->screen, nnn_pos);
}

int draw_block(Gfx *gfx, char colour, int x, int y)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	SDL_BlitSurface(gfx->block[colour], NULL, gfx->screen, &rect);
	return 0;
}


