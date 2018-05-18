#Go into sub directories
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = tetron
tetron_SOURCES = \
				src/tetron.c \
				src/tetron.h \
				src/sdl/gfx.c \
				src/sdl/gfx.h \
				src/sdl/input.c \
				src/sdl/input.h \
				src/sdl/sys.c \
				src/sdl/sys.h

tetron_CPPFLAGS = @SDL_CFLAGS@
#LDADD goes after the objects
# windows compilation seems to require the libraries come after the objects
#LDFLAGS goes at the beginning
tetron_LDADD = @SDL_LIBS@

#docdir, 
#pkgdatadir = /usr/local/share/tetron
#pkglibdir = /usr/local/lib/tetron
tetron_CPPFLAGS += -DDIR=\"$(pkgdatadir)/\"

# We want this file to be put in /usr/local/share/tetron/block.bmp
dist_pkgdata_DATA = menu.bmp
dist_pkgdata_DATA += block.bmp


