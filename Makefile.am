## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.1 1999/02/10 16:12:10 kahn Exp kahn $

SUBDIRS = src

CLEANFILES=*~ *.o .deps/* using_gnome

EXTRA_DIST =	\
	guiTAR-0.1.4.lsm \
	guiTAR.desktop \
	guiTAR.pot \
	install.gnome \
	guitar.1 \
	BUGS

man_MANS=guitar.1

pot: 	msgs

msgs:
		xgettext -i src/*.c src/gtkmyfilesel/*.c --keyword=N_ -o guiTAR.pot

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-`date +"%y%m%d"`

install-exec-hook:
	sh ./install.gnome

uninstall:
	sh ./install.gnome uninstall

