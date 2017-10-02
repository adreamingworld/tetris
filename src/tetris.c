#include <time.h>

#include "tetris.h"
#include <gfx.h>
#include <input.h>

char shapes[] =  {
				0b00001111, /* I */
				0b01000111, /* J */
				0b00010111, /* L */
				0b00110011, /* O */
				0b00110110, /* S */
				0b01100011, /* Z */
				0b00100111, /* T */
			};

int mcos[] = {0, -1, 0};
int msin[] = {1, 0, -1};

typedef struct {
	Gfx gfx;
	unsigned short keys[0x10000];
	int quit;
	Board board;
	Shape shape;
} Game;

unsigned short keys[0x10000];

void init_board(Board *board, int x, int y, int w, int h)
{
	int i;

	board->x = x;
	board->y = y;
	board->w = w;
	board->h = h+1;
	board->data = malloc(w*(h+1));

	/* Fill bottom with blocks to help collision */
	for (i=0; i<w; i++)
		board->data[i + h*w] = 1;
}

void init_shape(Shape *shape, int x, int y)
{
	shape->x = x;
	shape->y = y;
	shape->type = rand()%7;
	shape->new_rotation = rand()%4;
}

int get_block_from_shape(Shape *s, int i, int *x, int *y)
{

	if (!(shapes[s->type] & 1<<i)) return 0;
	/* all shifted -1, 0*/	
	int nx = i%4;
	int ny = i/4;

	nx = nx -1;

	/* rotate */
	if (s->rotation) {
		int factor = s->rotation - 1;
		int rnx = mcos[factor]*nx - msin[factor]*ny;
		int rny = mcos[factor]*ny + msin[factor]*nx;
		nx = rnx;
		ny = rny;
	}

	*x = nx + s->x;
	*y = ny + s->y;

	return 1;
}

void draw_shape(Gfx *gfx, Shape *s, Board *b)
{
	int i;
	for (i=0; i<2*4; i++) {
		int x ;
		int y ;
		if (get_block_from_shape(s, i, &x, &y)) {
			draw_block(gfx, 1, x*32,y*32);
		}
	}
}

void process_input(Game *game, unsigned short keys[])
{
	if (keys[KEY_ESCAPE]) game->quit = 1;

	if (keys[KEY_UP]) {
		if (keys[KEY_UP] & 0x80) {
			keys[KEY_UP] &= 1;
			game->shape.new_rotation = (game->shape.new_rotation+1)%4;
		}
	}
	if (keys[KEY_DOWN]) {
		if (keys[KEY_DOWN] & 0x80) {
			keys[KEY_DOWN] &= 1;
			game->shape.type = (game->shape.type+1)%7;
		}
	}

	if (keys[KEY_LEFT]) {
		if (keys[KEY_LEFT] & 0x80) {
			keys[KEY_LEFT] &= 1;
			game->shape.vel_x = -1;
		}
	}

	if (keys[KEY_RIGHT]) {
		if (keys[KEY_RIGHT] & 0x80) {
			keys[KEY_RIGHT] &= 1;
			game->shape.vel_x = 1;
		}
	}

}

void set_shape(Shape *s, Board *b)
{
	int i;
	int x,y;

	for (i=0; i<2*4; i++) {
		if (get_block_from_shape(s, i, &x, &y) ) {
			b->data[x + y*b->w] = 1;
		}
	}
}

void check_collision(Board *b, Shape *s)
{
	int i;
	int x,y;
	int hit = 0;
	char old_rotation = s->rotation;
	s->rotation = s->new_rotation;

	for (i=0; i<2*4; i++) {
		if (get_block_from_shape(s, i, &x, &y) ) {
			if (y<0) continue; /* We don't care if it's too high up */
			if (x+s->vel_x < 0  ||
				x+s->vel_x >= b->w
				) {
				hit = 1;
				break;
			}
			if (b->data[x+s->vel_x + (y+s->vel_y)*b->w]) {
				s->rotation = old_rotation;
				get_block_from_shape(s, i, &x, &y);
				hit = 1;
				if (b->data[x + (y+1)*b->w]) {
					set_shape(s, b);
					init_shape(s, 4, -4);
					return ; /* Nothing else needed */
				}	
				break; 
			}
		}
	}

	if (!hit) {
		s->x += s->vel_x;
	} else {
		s->rotation = old_rotation;
		s->new_rotation = old_rotation;
	}
	/* y whatever, if not set */
	s->y += s->vel_y;
	s->vel_x = 0;
	s->vel_y = 0;
}
void draw_board(Gfx *gfx, Board *b) 
{
	int i;

	for (i=0; i< b->w*b->h; i++) {
		int x = i%b->w;
		int y = i/b->w;

		if (b->data[i]) draw_block(gfx, 1, x*32, y*32);
	}
}

int main()
{
	int i=0;
	Game game = {0};

	srand(time(NULL));

	init_shape(&game.shape, 4,4);
	init_board(&game.board, 0,0, 10, 20);
	init_gfx(&game.gfx, "Tetris", 10*32, 20*32);

	while (!game.quit) {
		get_input(game.keys);
		process_input(&game, game.keys); 

		/* Calculate positions */
		check_collision(&game.board, &game.shape);
		/* Draw shape */
		draw_shape(&game.gfx, &game.shape, &game.board);

		/* Draw board */
		draw_board(&game.gfx, &game.board);

		/* Update display */
		gfx_flip(&game.gfx);
		gfx_clear(&game.gfx);

		if (i%20 == 0) game.shape.vel_y = 1;
		i++;
	}
}
