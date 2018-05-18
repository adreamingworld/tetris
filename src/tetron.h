#ifndef TETRIS_H
#define TETRIS_H

#include <stdio.h>

typedef struct {
	char type;
	char colour;
	char rotation;
	char new_rotation;
	int x,y;
	int vel_x, vel_y;
} Shape;

typedef struct {
	char active;
	float x, y;
	float vx, vy;
	char type;
} Particle;

typedef struct {
	int x,y;
	int w,h;
	char **data;
	char *line_status;
	Particle *particles;
} Board;

void init_board(Board *board, int x, int y, int w, int h);
void init_shape(Shape *shape, int x, int y);

#endif /* TETRIS_H */

