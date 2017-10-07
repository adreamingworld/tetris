#Go into sub directories
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = tetris
tetris_SOURCES = \
				src/tetris.c \
				src/tetris.h \
				src/sdl/gfx.c \
				src/sdl/gfx.h \
				src/sdl/input.c \
				src/sdl/input.h 

tetris_CPPFLAGS = @SDL_CFLAGS@
tetris_LDFLAGS = @SDL_LIBS@
