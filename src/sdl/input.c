#include "input.h"

int get_input(unsigned short keys[])
{
	SDL_Event event;

	if (SDL_PollEvent( &event ) ){
		switch (event.type) {
			case SDL_KEYDOWN:
				keys[event.key.keysym.sym] = 0x81;
				break;
			case SDL_KEYUP:
				keys[event.key.keysym.sym] = 0x00;
				break;
		}
	}


	return 1;
}
