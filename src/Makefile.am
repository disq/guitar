
SUBDIRS = gtkmyfilesel

CLEANFILES=*~ *.o .deps/* guitar

LDADD = gtkmyfilesel/libgtkmyfilesel.a

bin_PROGRAMS = guitar

guitar_DEPENDENCIES = gtkmyfilesel/libgtkmyfilesel.a

guitar_SOURCES = \
  dnd.c \
  dnd.h \
  gladesrc.c \
  gladesrc.h \
  menu.c \
  menu.h \
  programs.c \
  programs.h \
  main.c \
  main.h \
  common.h

EXTRA_DIST =	\
	logo.xpm \
	icon.xpm \
	guiTAR.xpm
