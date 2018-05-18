/**
	@file tetron.c
	@todo 	Make a drop function. Make a ghost shape that shows where
			the shape will land.
	@todo	make it so the shape is drawn with the board
			this way we can align it with it. We can draw a portion
			of the board, and the shape where it should be in the board.
	@todo	Midi music. Tempo speeds up with the game.
	@todo	We need to check if lines are full in the right order.
*/

#include <time.h>

/* From configure.ac */
#include "config.h"

#include "tetron.h"
#include "sdl/gfx.h"
#include "sdl/input.h"
#include "sdl/sys.h"

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
	int state; /*0 = menu; 1 = game*/
	Board board;
	Shape shape;
	int w,h;
	unsigned int ticks;
	unsigned int start_ticks;
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

	board->line_status = calloc(1, h);

	board->particles = calloc( sizeof(Particle)* w*(h+1), 1);
	
	for (i=0; i<(h)*(w-2); i++) {
		int x = 1+ i%(w-2);
		int y = i/(w-2);
		//init_particle(&board->particles[i], x, y, rand()%7);
		init_particle(&board->particles[i], x, y, 2);
	}

	for (i = 0; i < h+1; i++) {
		board->data[i] = calloc(sizeof(char) * (w), 1);
	}

	/* Fill bottom with blocks to help collision */
	for (i=0; i<(w)*(h+1); i++) {
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
	shape->type = 1 + rand()%6;
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
			if (y < 0) continue; 
			draw_block(gfx, s->type, (x)*32,y*32);
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
	if (keys[KEY_ESCAPE]) {
		keys[KEY_ESCAPE] &= 1;
		game->state = 0; /* Back to the menu */
	}

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

/** 
	@brief
	Destroys row of blocks when filled.
 */
void destroy_line(Board *b, int l)
{
	int i;
	/*Unmark for death, so this line can be reused */
	b->line_status[l] = 0;
	for (i=0; i<b->w; i++) {
			Particle *p = &b->particles[i+l*b->w];
		init_particle(p, i, l, b->data[l][i] - 1);
	}

	/** 	 
		So the top row should always be kept clear? Otherwise we would copy blocks onto the lower ones.
	*/
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
			//destroy_line(b, line);
			/* Mark for death */
			b->line_status[line] = 1;
		}
	}

	for (i=0; i<b->h; i++) {
		/* If marked for death */
		if (b->line_status[i]) {
			destroy_line(b, i);
		}
	}
}

void draw_board(Gfx *gfx, Board *b) 
{
	int i;
	for (i=0; i< (b->w)*b->h; i++) {
		int x = i%(b->w);
		int y = i/(b->w);
		/* -1 becasue 0 means empty */
		if (b->data[y][x]) draw_block(gfx, b->data[y][x]-1, (x)*32, y*32);
	}
}
void draw_particle(Gfx *gfx, Game *game, Particle *p, int move) {
	draw_block(gfx, p->type, p->x, p->y);
	if (!move) return;
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

int play(Game *game) {

	while (game->state == 1) {
		get_input(game->keys);
		process_input(game, game->keys); 

		/* Calculate positions */
		/* Draw board */
		draw_board(&game->gfx, &game->board);

		/* Draw shape */
		draw_shape(&game->gfx, &game->shape, &game->board);

		/* Draw Particles */
		int j;
		for (j=0; j< game->board.w * game->board.h;j++) {
			if (game->board.particles[j].active)
				draw_particle(&game->gfx, game, &game->board.particles[j], 1);
		}

		/* Update display */
		gfx_flip(&game->gfx);
		gfx_clear(&game->gfx);

		unsigned int current_time = sys_get_ticks();
		if (current_time > game->start_ticks + 1000) {
			game->start_ticks = sys_get_ticks();
			if (check_collision(&game->board, game->shape.type, game->shape.rotation, game->shape.x, game->shape.y+1) ) {
				game->shape.y += 1;
			} else {
				set_shape(&game->shape, &game->board);
				/* New shape */
				init_shape(&game->shape, 4,0);
			}
		}
	}
}

int menu(Game *game) {
	int selected = 0;


	while (!game->quit) {
		get_input(game->keys);

		if (game->keys[KEY_DOWN]) {
			if (game->keys[KEY_DOWN] & 0x80) {
				game->keys[KEY_DOWN] &= 0x01;
				selected++;
			}
		}
		if (game->keys[KEY_UP]) {
			if (game->keys[KEY_UP] & 0x80) {
				game->keys[KEY_UP] &= 0x01;
				selected--;
			}
		}

		/* Sort selected bounds */
		if (selected < 0) selected = 2;
		if (selected > 2) selected = 0;

		if (game->keys[KEY_RETURN]) {
			switch (selected) {
				case 0:
					init_shape(&game->shape, 4,0);
					init_board(&game->board, 0,0, 12, 22);
					game->state = 1;
					play(game);
				break;
				case 1:
					game->state = 1;
					play(game);
				break;
				case 2:
					game->quit = 1;
				break;
			}
		}

		draw_board(&game->gfx, &game->board);
		draw_shape(&game->gfx, &game->shape, &game->board);
		/*Draw Particles */
		/** @todo this should be part of draw board function */
		int j;
		for (j=0; j< game->board.w * game->board.h;j++) {
			if (game->board.particles[j].active)
				draw_particle(&game->gfx, game, &game->board.particles[j], 0);
		}
		/* Draw menu */
		draw_menu(&game->gfx, selected, sys_get_ticks());
		gfx_flip(&game->gfx);
		gfx_clear(&game->gfx);
	}
}


int main(int argc, char *argv[])
{
	Game game = {0};

	srand((unsigned) time(NULL));

	printf(PACKAGE_NAME"-"VERSION"\n");

	init_shape(&game.shape, 4,0);
	init_board(&game.board, 0,0, 12, 22);
	int sidebar_size = 16*2*4;
	init_gfx(&game.gfx, "Tetron", game.board.w*32 + sidebar_size, game.board.h*32);
	game.w = game.gfx.w;
	game.h = game.gfx.h;

	game.start_ticks = sys_get_ticks();
	/* main loop */
	menu(&game);
}
