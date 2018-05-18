#Go into sub directories
AUTOMAKE_OPTIONS = subdir-objects

ACLOCAL_AMFLAGS = -I m4 --install
EXTRA_DIST = m4/NOTES

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
#Pass data directory path to the program
tetron_CPPFLAGS += -DDATA_DIR=\"$(pkgdatadir)/\"

tetron_res_dir = $(pkgdatadir)/res
# We want this file to be put in /usr/local/share/tetron/block.bmp
#put stuff in /usr/local/share/tetron/res
tetron_res__DATA = res/block.bmp
tetron_res__DATA += res/menu.bmp


