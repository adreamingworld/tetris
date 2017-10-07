/**
	@file tetris.c
	@todo 	Make a drop function. Make a ghost shape that shows where
			the shape will land.
	@todo	Midi music. Tempo speeds up with the game.
	@todo	We need to check if lines are full in the right order.
*/

#include <time.h>

#include "tetris.h"
#include "sdl/gfx.h"
#include "sdl/input.h"

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
	int w,h;
} Game;

unsigned short keys[0x10000];

void init_particle(Particle *p, int x, int y, char type) 
{
	p->active = 1;
	p->type = type;
	p->x = x*32;
	p->y = y*32;
	p->vy = -(1.0 + (float)(rand()%20) / 10.0);
	p->vx = -1.0 + ((float)(rand()%20) / 10.0);
}
void init_board(Board *board, int x, int y, int w, int h)
{
	int i;

	board->x = x;
	board->y = y;
	board->w = w;
	board->h = h;
	board->data = calloc((h+1) * sizeof(char *), 1);

	board->particles = calloc( sizeof(Particle)* w*(h+1), 1);
	Particle *p = &board->particles[2];
	
	for (i=0; i<(h+1)*w; i++) {
		int x = i%w;
		int y = i/w;
		init_particle(&board->particles[i], x, y, rand()%7);
	}

	for (i = 0; i < h+1; i++) {
		board->data[i] = malloc(sizeof(char) * w);
	}

	/* Fill bottom with blocks to help collision */
	for (i=0; i<w*(h+1); i++) {
		int x = i%w;
		int y = i/w;
		if (y==h || x==0 || x==w-1)
			board->data[y][x] = 1;
	}
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
			draw_block(gfx, s->type, x*32,y*32);
		}
	}
}

int check_collision(Board *b, char type, char rotation, int x, int y)
{
	int i;
	int nx,ny;
	int hit = 0;

	Shape s;
	s.x = x;
	s.y = y;
	s.rotation = rotation;
	s.type = type;

	for (i=0; i<2*4; i++) {
		if (get_block_from_shape(&s, i, &nx, &ny) ) {
			/* If off screen then we cannot collide */
			if (ny<0) continue;
			if (b->data[ny][nx]) {
				return 0; 
			}
		}
	}

	return 1;
}
void process_input(Game *game, unsigned short keys[])
{
	if (keys[KEY_ESCAPE]) game->quit = 1;

	if (keys[KEY_UP]) {
		if (keys[KEY_UP] & 0x80) {
			keys[KEY_UP] &= 1;
			if (check_collision(&game->board, game->shape.type, (game->shape.rotation+1)%4, game->shape.x, game->shape.y) ) {
				game->shape.rotation = (game->shape.rotation+1)%4;
			}
		}
	}
	if (keys[KEY_DOWN]) {
		if (keys[KEY_DOWN] ) {
		//	keys[KEY_DOWN] &= 1;
			if (check_collision(&game->board, game->shape.type, game->shape.rotation, game->shape.x, game->shape.y+1) ) {
				game->shape.y += 1;
			}
		}
	}

	if (keys[KEY_LEFT]) {
		if (keys[KEY_LEFT] & 0x80) {
			keys[KEY_LEFT] &= 1;
			if (check_collision(&game->board, game->shape.type, game->shape.rotation, game->shape.x-1, game->shape.y) ) {
				game->shape.x -= 1;
			}
		}
	}

	if (keys[KEY_RIGHT]) {
		if (keys[KEY_RIGHT] & 0x80) {
			keys[KEY_RIGHT] &= 1;
			if (check_collision(&game->board, game->shape.type, game->shape.rotation, game->shape.x+1, game->shape.y) ) {
				game->shape.x += 1;
			}
		}
	}

}

int check_line(Board *b, int l)
{
	int i;
	for (i=0; i<b->w; i++) {
		if (b->data[l][i] == 0) return 0;
	}
	return 1;
}

void destroy_line(Board *b, int l)
{
	int i;
	for (i=0; i<b->w; i++) {
			Particle *p = &b->particles[i+l*b->w];
		init_particle(p, i, l, b->data[l][i] - 1);
	}

	for (i = l; i > 1; i--) {
		/* Swap this one with the one before */
		memcpy(b->data[i], b->data[i-1], b->w);
	}
}

void set_shape(Shape *s, Board *b)
{
	int i;
	int x,y;
	int lines_to_check[4]={0}; /* Can only be a maximum of 4 lines to check */
	int line_count=0;

	for (i=0; i<2*4; i++) {
		if (get_block_from_shape(s, i, &x, &y) ) {
			if (y<0) continue;
			b->data[y][x] = s->type+1;
			int j=0;
			for (j=0; j<line_count; j++) if (lines_to_check[j] == y+1) break;
			if (j==line_count || line_count==0) lines_to_check[line_count++] = y+1; /* Add one because 0 means invalid */
		}
	}

	/* check lines to see if full */
	for (i=0; i<line_count; i++) {
		if (lines_to_check[i]-1 > 1)
		if (check_line(b, lines_to_check[i]-1)) {
			int line = lines_to_check[i]-1;
			destroy_line(b, line);
		}
	}
}

void draw_board(Gfx *gfx, Board *b) 
{
	int i;
	for (i=0; i< b->w*b->h; i++) {
		int x = i%b->w;
		int y = i/b->w;
		/* -1 becasue 0 means empty */
		if (b->data[y][x]) draw_block(gfx, b->data[y][x]-1, x*32, y*32);
	}
}
void draw_particle(Gfx *gfx, Game *game, Particle *p) {
	draw_block(gfx, p->type, p->x, p->y);
	p->x += p->vx;
	p->y += p->vy;
	p->vy += 0.05;

	if (
		p->x > game->w ||
		p->x < -32 ||
		p->y < -32 ||
		p->y > game->h
		) p->active = 0;
}

int main()
{
	int i=0;
	Game game = {0};

	srand(time(NULL));

	init_shape(&game.shape, 4,0);
	init_board(&game.board, 0,0, 12, 22);
	init_gfx(&game.gfx, "Tetris", game.board.w*32, game.board.h*32);
	game.w = game.gfx.w;
	game.h = game.gfx.h;

	while (!game.quit) {
		get_input(game.keys);
		process_input(&game, game.keys); 

		/* Calculate positions */
		/* Draw board */
		draw_board(&game.gfx, &game.board);

		/* Draw shape */
		draw_shape(&game.gfx, &game.shape, &game.board);

		/* Draw Particles */
		int j;
		for (j=0; j< game.board.w * game.board.h-1; j++) {
			if (game.board.particles[j].active)
				draw_particle(&game.gfx, &game, &game.board.particles[j]);
		}

		/* Update display */
		gfx_flip(&game.gfx);
		gfx_clear(&game.gfx);

		if (i%40 == 0) {
			if (check_collision(&game.board, game.shape.type, game.shape.rotation, game.shape.x, game.shape.y+1) ) {
				game.shape.y += 1;
			} else {
				set_shape(&game.shape, &game.board);
				/* New shape */
				init_shape(&game.shape, 4,0);
			}
		}
		i++;
	}
}
