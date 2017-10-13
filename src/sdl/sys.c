#include "sys.h"

unsigned int sys_get_ticks()
{
	return SDL_GetTicks();
}
