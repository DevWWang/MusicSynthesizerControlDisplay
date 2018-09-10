#ifndef __XFS__H
#define __XFS__H

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define XFS_DEFAULT_FONT "7x14"
#define XFS_INSTGRAPH_FONT "4x6"

#define XFS_OPERATORBUTTON_CLASS "XfsOperatorButton"
#define BUTTON_WIDTH 105
#define BUTTON_HEIGHT 25

#define C_RED "rgb:ff/00/00"
#define C_GREEN "rgb:00/ff/00"
#define C_GREEN2 "rgb:00/ee/00"
#define C_BLUE "rgb:00/00/ff"
#define C_GRAY1 "rgb:40/40/40"
#define C_GRAY2 "rgb:80/80/80"
#define C_GRAY3 "rgb:c0/c0/c0"
#define C_GRAY4 "rgb:d0/d0/d0"
#define C_GRAY5 "rgb:e0/e0/e0"
#define C_GRAY6 "rgb:a0/a0/a0"

typedef struct _XfsWindowState {
	struct _XfsWindowState *next;
	Window window;
	void *data;
} XfsWindowState;

#define XfsDrawRectShadow(d, W, g, x, y, w, h) \
		XDrawRectangle(d, W, g, x, y, w - 1, h - 1); \
		XDrawRectangle(d, W, g, x + 1, y + 1, w - 2, h - 2)

/* Xfs Calls */
Display *XfsInit(char *display_name);

/* XPM stuff */
void XfsDrawXpmImage256(Display *display, char **xpm, Pixmap *pixmap_return, GC *gc_return, int *width_return, int *height_return);

/* Add Node */
XfsWindowState *XfsAddNode(Window window, void **root_list, size_t n);

/* Event Handler per window */
void XfsOperatorButtonEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data);

#endif