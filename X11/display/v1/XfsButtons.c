#include <X11/Xlib.h>
#include "Xfs.h"
#include <stdlib.h>
#include <strings.h>

#define RBDIAM 11
#define RB_X 8

Colormap colormap;
int blackpixel;
int whitepixel;

static GC gc = NULL;
static GC gc_up = NULL;
static GC gc_down = NULL;
static GC gc_green = NULL;
static GC gc_gray3 = NULL;

int operator_number = -1;

typedef struct _XfsOperatorState {
	struct _XfsOperatorState *next;
	Window window;
	int activated;
} XfsOperatorState;

static XfsOperatorState *root_button_list = NULL;

/* Xfs Calls */
Display *XfsInit(char *display_name) {
	Display *display = XOpenDisplay(display_name);
	colormap = DefaultColormap(display, DefaultScreen(display));

	/* initialize Color */
	blackpixel = BlackPixel(display, DefaultScreen(display));
	whitepixel = WhitePixel(display, DefaultScreen(display));

	return display;
}

void XfsEventToolkitHandler(Display *display, XEvent *e, Window *winret, void *data) {
	XClassHint class_hint;
	*winret = (Window)NULL;

	if (XGetClassHint(display, e->xany.window, &class_hint) == BadWindow || !class_hint.res_class) {
		return;
	}
	if (!strcmp(class_hint.res_class, XFS_OPERATORBUTTON_CLASS)) {
		XfsOperatorButtonEvent(display, e, winret, &class_hint, data);
	}
}

/* Add Node */
XfsWindowState *XfsAddNode(Window window, void **root_list, size_t n) {
	XfsWindowState *winstate;

	winstate = (XfsWindowState *)malloc(n);
	memset(winstate, 0, n);

	winstate->window = window;

	winstate->next = *root_list;
	*root_list = winstate;

	return winstate;
}

/* Create Operator Button Element */
Window XfsCreateOperatorButton(Display *display, Window parent, int x, int y, int width, int height, char *label) {
	Window w;
	XClassHint class_hint;
	XGCValues gcv;
	XColor color_up, color_down, color_green, color_gray3;

	//XfsOperatorState *operator_state;
	if (!gc) {
		XParseColor(display, colormap, C_GRAY3, &color_gray3);
		XAllocColor(display, colormap, &color_gray3);
		gcv.background = blackpixel;
		gcv.foreground = color_gray3.pixel;
		gc_gray3 = XCreateGC(display, DefaultRootWindow(display), GCForeground | GCBackground, &gcv);

		XParseColor(display, colormap, C_GRAY5, &color_up);
		XAllocColor(display, colormap, &color_up);
		gcv.background = blackpixel;
		gcv.foreground = color_up.pixel;
		gc_up = XCreateGC(display, DefaultRootWindow(display), GCForeground | GCBackground, &gcv);

		XParseColor(display, colormap, C_GRAY2, &color_down);
		XAllocColor(display, colormap, &color_down);
		gcv.background = blackpixel;
		gcv.foreground = color_down.pixel;
		gc_down = XCreateGC(display, DefaultRootWindow(display), GCForeground | GCBackground, &gcv);

		gcv.background = color_up.pixel;
		gcv.foreground = blackpixel;
		gc = XCreateGC(display, DefaultRootWindow(display), GCForeground | GCBackground, &gcv);

		XParseColor(display, colormap, C_GREEN, &color_green);
		XAllocColor(display, colormap, &color_green);
		gcv.background = blackpixel;
		gcv.foreground = color_green.pixel;
		gc_green = XCreateGC(display, DefaultRootWindow(display), GCForeground | GCBackground, &gcv);
	}

	w = XCreateSimpleWindow(display, parent, x, y, width, height, 1, blackpixel, color_up.pixel);
	class_hint.res_name = label;
	class_hint.res_class = XFS_OPERATORBUTTON_CLASS;
	XSetClassHint(display, w, &class_hint);

	XSelectInput(display, w, ExposureMask | ButtonPressMask | ButtonReleaseMask);
	XMapWindow(display, w);

	/* Add the new radio button to the link list */

#if 0
	if (!(operator_state = (XfsOperatorState *)malloc(sizeof(XfsOperatorState)))) {
	fprintf(stderr, "Unable to allocate new radio control to link list\n");
	return w;
	}
#endif
	//memset(operator_state, 0, sizeof(XfsOperatorState));
	//operator_state->window = w;
	//XfsAddNode((XfsWindowState *)operator_state, (XfsWindowState **)&root_button_list);

	XfsAddNode(w, (void **)&root_button_list, sizeof(XfsOperatorState));

	return w;
}

int XfsGetOperatorButtonState(Window window, int toggle) {
	XfsOperatorState *temp;

	temp = root_button_list;

	while (temp) {
		if (temp->window == window) {
			if (toggle) {
				temp->activated = temp->activated?0:1;
			}
			return temp->activated;
		}
		temp = temp->next;
	}
	fprintf(stderr, "err\n");

	return -1;
}

int XfsGetOperatorButtonNumber(int number, int state) {

	if (state == 1) {
		printf("Op_Nb =  %d\n", number);
		return number;
	}
}

void XfsDrawOperatorButton(Display *display, Window w, int x, int y, int width, int height, char *label, int activate) {
	XFontStruct *font;
	XTextItem ti[1];
	int state;

	font = XLoadQueryFont(display, XFS_DEFAULT_FONT);
	ti[0].chars = label;
	ti[0].nchars = strlen(label);
	ti[0].delta = 0;
	ti[0].font = font->fid;

	state = XfsGetOperatorButtonState(w, activate?1:0);
	XfsDrawRectShadow(display, w, activate?gc_down:gc_up, 0, 0, width, height);

	XFillArc(display, w, state?gc_green:gc_gray3, RB_X, (height - RBDIAM) / 2, RBDIAM, RBDIAM, 0, 23040);
	XDrawArc(display, w, gc, RB_X, (height - RBDIAM) / 2, RBDIAM, RBDIAM, 0, 23040);
	XDrawText(display, w, gc, RB_X * 2 + RBDIAM, (height - (font->ascent + font->descent)) / 2 + font->ascent, ti, 1);

	XUnloadFont(display, font->fid);
}

void XfsOperatorButtonEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data) {
	Window root_return;
	int x_return, y_return;
	unsigned int width_return, height_return;
	unsigned int border_width_return;
	unsigned int depth_return;

	XGetGeometry(display, e->xany.window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);

	*winret = e->xany.window;

	switch(e->type) {
		case ButtonPress:
			if (e->xbutton.button == 1) {
				XfsDrawOperatorButton(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 1);
			}
			break;
		case ButtonRelease:
			if (e->xbutton.button == 1) {
				XfsDrawOperatorButton(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;
		case Expose:
			if (e->xexpose.count == 0) {
				XfsDrawOperatorButton(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;

		default:
			*winret = 0;
	}
}
