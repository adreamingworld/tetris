#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include "../tetris.h"

#define KEY_UP		SDLK_UP
#define KEY_DOWN	SDLK_DOWN
#define KEY_LEFT	SDLK_LEFT
#define KEY_RIGHT	SDLK_RIGHT

#define KEY_RETURN	SDLK_RETURN
#define KEY_ESCAPE	SDLK_ESCAPE

int get_input(unsigned short keys[]);

#endif /* INPUT_H */

