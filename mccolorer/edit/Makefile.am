AM_CFLAGS = $(GLIB_CFLAGS)

if USE_EDIT
noinst_LIBRARIES = libedit.a
else
noinst_LIBRARIES =
endif

if USE_COLORER
libedit_a_SOURCES = \
	bookmark.c edit.c editcmd.c editwidget.c editdraw.c editkeys.c \
	editmenu.c editoptions.c editcmddef.h edit.h edit-widget.h \
	editlock.c editlock.h syntax.c wordproc.c \
	syntax-colorer-wrapper.c syntax-colorer.cpp
else
libedit_a_SOURCES = \
	bookmark.c edit.c editcmd.c editwidget.c editdraw.c editkeys.c \
	editmenu.c editoptions.c editcmddef.h edit.h edit-widget.h \
	editlock.c editlock.h syntax.c wordproc.c
endif

EXTRA_DIST = ChangeLog
