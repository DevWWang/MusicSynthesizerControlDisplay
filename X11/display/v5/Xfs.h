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

#define SAVE_BUTTON_WIDTH 60
#define SAVE_BUTTON_HEIGHT 20

#define PRESET_BUTTON_WIDTH 90
#define PRESET_BUTTON_HEIGHT 20
// Operator No.
#define OPERATOR_1 1
#define OPERATOR_2 2
#define OPERATOR_3 3
#define OPERATOR_4 4
#define OPERATOR_5 5
#define OPERATOR_6 6
// Waveform Types
#define TYPE_SINE 0
#define TYPE_SQUARE 1
#define TYPE_SAWTOOTH 2
#define TYPE_TRIANGULAR 3
// Mode Types
#define TYPE_ONESHOT 0
#define TYPE_LOOP 1
// Message Types
#define TYPE_PRESET 1
#define TYPE_USER_SETTING 2

#define XFS_SLIDER_CLASS "XfsSlider"
#define SLIDER_WIDTH 640
#define SLIDER_HEIGHT 15
// Slider Types
#define TYPE_A 1
#define TYPE_T 2

#define XFS_OPERATOR_SLIDER_CLASS "XfsOperatorSlider"
#define OPERATOR_SLIDER_WIDTH 10
#define OPERATOR_SLIDER_HEIGHT 200
// Operator Slider Types
#define TYPE_COEF 1
#define TYPE_ARR 2

#define XFS_LEVELBOX_CLASS "XfsLevelBox"
#define	LEVELBOX_WIDTH 150
#define LEVELBOX_HEIGHT 25

#define XFS_LEVELDRAWING_CLASS "XfsLevelDrawing"
#define XFS_DRAWING_WIDTH 480
#define XFS_DRAWING_HEIGHT 280

#define XFS_FMDISPLAY_CLASS "XfsFMDisplay"
#define XFS_DISPLAY_WIDTH 200
#define XFS_DISPLAY_HEIGHT 260

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
void XfsSliderEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data);
void XfsLevelBoxEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data);
void XfsOperatorSliderEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data);
void XfsLevelDrawingEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data);
void XfsFMDisplayEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data);

#endif