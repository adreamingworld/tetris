#Go into sub directories
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = tetris
tetris_SOURCES = \
				src/tetris.c \
				src/tetris.h \
				src/sdl/gfx.c \
				src/sdl/gfx.h \
				src/sdl/input.c \
				src/sdl/input.h \
				src/sdl/sys.c \
				src/sdl/sys.h

tetris_CPPFLAGS = @SDL_CFLAGS@
#LDADD goes after the objects
# windows compilation seems to require the libraries come after the objects
#LDFLAGS goes at the beginning
tetris_LDADD = @SDL_LIBS@

#docdir, 
#pkgdatadir = /usr/local/share/tetris
#pkglibdir = /usr/local/lib/tetris
tetris_CPPFLAGS += -DDIR=\"$(pkgdatadir)/\"

# We want this file to be put in /usr/local/share/tetris/block.bmp
dist_pkgdata_DATA = menu.bmp
dist_pkgdata_DATA += block.bmp


