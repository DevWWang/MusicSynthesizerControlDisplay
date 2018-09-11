#include <X11/Xlib.h>
#include "Xfs.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <memory.h>

// Button Light
#define RBDIAM 11
#define RB_X 8

// Slider Control
#define SLIDER_CONTROL_WIDTH 8
#define SLIDER_CONTROL_HEIGHT 15

// Operator Slider Control
#define OPERATOR_SLIDER_CONTROL_WIDTH 10
#define OPERATOR_SLIDER_CONTROL_HEIGHT 8

// Level Setting
#define LB_X 5
#define LB_Y 5
#define LB_WIDTH 5
#define LB_HEIGHT 13

Colormap colormap;
int blackpixel;
int whitepixel;

static GC gc = NULL;
static GC gc_up = NULL;
static GC gc_down = NULL;
static GC gc_green = NULL;
static GC gc_gray3 = NULL;

// Oeprator
int operator_selected = 0;
int waveform_selected = 0;
int mode_selected = 0;

// Slider
int slider_control_x = 0;
int slider_control_y = 0;

// Operator Slider
int operator_slider_control_x = 0;
int operator_slider_control_y = 0;
int operator_mod = 0;
int operator_coef = 0;
int operator_arr = 0;

// Level
int level_amplitude = 0;
int level_time_interval = 0;
int amp_level_1, amp_level_2, amp_level_3, amp_level_4, amp_level_5, amp_level_6, amp_level_7;
int time_level_1, time_level_2, time_level_3, time_level_4, time_level_5, time_level_6, time_level_7;

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
	if (!strcmp(class_hint.res_class, XFS_SLIDER_CLASS)) {
		XfsSliderEvent(display, e, winret, &class_hint, data);
	}
	if (!strcmp(class_hint.res_class, XFS_LEVELBOX_CLASS)) {
		XfsLevelBoxEvent(display, e, winret, &class_hint, data);
	}
	if (!strcmp(class_hint.res_class, XFS_OPERATOR_SLIDER_CLASS)) {
		XfsOperatorSliderEvent(display, e, winret, &class_hint, data);
	}
	if (!strcmp(class_hint.res_class, XFS_LEVELDRAWING_CLASS)) {
		XfsLevelDrawingEvent(display, e, winret, &class_hint, data);
	}
	if (!strcmp(class_hint.res_class, XFS_FMDISPLAY_CLASS)) {
		XfsFMDisplayEvent(display, e, winret, &class_hint, data);
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

/***********************************************************************************************************/
/***************************************** OPERATOR BUTTON *************************************************/
/***********************************************************************************************************/

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

void XfsSetOperatorSelected(int op_1, int op_2, int op_3, int op_4, int op_5, int op_6) {
	if (op_1 == 1 && op_2 == 0 && op_3 == 0 && op_4 == 0 && op_5 == 0 && op_6 == 0) {
		operator_selected = OPERATOR_1;
		//printf("operator_selected = %d\n", operator_selected);
	}
	if (op_1 == 0 && op_2 == 1 && op_3 == 0 && op_4 == 0 && op_5 == 0 && op_6 == 0) {
		operator_selected = OPERATOR_2;
		//printf("operator_selected = %d\n", operator_selected);
	}
	if (op_1 == 0 && op_2 == 0 && op_3 == 1 && op_4 == 0 && op_5 == 0 && op_6 == 0) {
		operator_selected = OPERATOR_3;
		//printf("operator_selected = %d\n", operator_selected);
	}
	if (op_1 == 0 && op_2 == 0 && op_3 == 0 && op_4 == 1 && op_5 == 0 && op_6 == 0) {
		operator_selected = OPERATOR_4;
		//printf("operator_selected = %d\n", operator_selected);
	}
	if (op_1 == 0 && op_2 == 0 && op_3 == 0 && op_4 == 0 && op_5 == 1 && op_6 == 0) {
		operator_selected = OPERATOR_5;
		//printf("operator_selected = %d\n", operator_selected);
	}
	if (op_1 == 0 && op_2 == 0 && op_3 == 0 && op_4 == 0 && op_5 == 0 && op_6 == 1) {
		operator_selected = OPERATOR_6;
		//printf("operator_selected = %d\n", operator_selected);
	}
}

int XfsGetOepratorNumber() {
	return operator_selected;
}

void XfsSetWaveformSelected(int sine, int square, int sawtooth, int triangular) {
	if (sine == 1 && square == 0 && sawtooth == 0 && triangular == 0) {
		waveform_selected = TYPE_SINE;
		//printf("Sine\n");
	}
	if (sine == 0 && square == 1 && sawtooth == 0 && triangular == 0) {
		waveform_selected = TYPE_SQUARE;
		//printf("Square\n");
	}
	if (sine == 0 && square == 0 && sawtooth == 1 && triangular == 0) {
		waveform_selected = TYPE_SAWTOOTH;
		//printf("Sawtooth\n");
	}
	if (sine == 0 && square == 0 && sawtooth == 0 && triangular == 1) {
		waveform_selected = TYPE_TRIANGULAR;
		//printf("Triangular\n");
	}
}

int XfsGetWavefrom() {
	return waveform_selected;
}

void XfsSetModeSelected(int standard, int segmentation_loop) {
	if (standard == 1 && segmentation_loop == 0) {
		mode_selected = TYPE_ONESHOT;
		//printf("One-shot\n");
	}
	if (standard == 0 && segmentation_loop == 1) {
		mode_selected = TYPE_LOOP;
		//printf("Segmentation Loop\n");
	}
}

int XfsGetMode() {
	return mode_selected;
}

void XfsDrawOperatorButton(Display *display, Window w, int x, int y, int width, int height, char *label, int activate) {
	XFontStruct *font;
	XTextItem ti[1];
	int state;
	const char *save = "SAVE";

	font = XLoadQueryFont(display, XFS_DEFAULT_FONT);
	ti[0].chars = label;
	ti[0].nchars = strlen(label);
	ti[0].delta = 0;
	ti[0].font = font->fid;

	if (strcmp(label, save) == 0) {
		state = 0;
	}
	else {
		state = XfsGetOperatorButtonState(w, activate?1:0);
	}

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

/***********************************************************************************************************/
/***************************************** SLIDER SETTING **************************************************/
/***********************************************************************************************************/

/* Create Operator Button Element */
Window XfsCreateSlider(Display *display, Window parent, int x, int y, int width, int height, char *label) {
	Window w;
	XClassHint class_hint;
	XGCValues gcv;
	XColor color_up, color_down, color_gray3;

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

	}

	w = XCreateSimpleWindow(display, parent, x, y, width, height, 1, blackpixel, color_up.pixel);
	class_hint.res_name = label;
	class_hint.res_class = XFS_SLIDER_CLASS;
	XSetClassHint(display, w, &class_hint);

	XSelectInput(display, w, ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask);
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

void XfsSetSliderControlPos(int x, int y) {
	slider_control_x = x;
	slider_control_y = y;
}

int XfsGetSliderControlPosX() {
	return slider_control_x;
}

int XfsGetSliderControlPosY() {
	return slider_control_y;
}

int XfsCalculateSlider(int type, int value) {
	int temp = value / 5;
	if (type == TYPE_A) {
		if (value < 0) {
			temp = 0;
		}
		if (value > SLIDER_WIDTH) {
			temp = 127;
		}
	}
	if (type == TYPE_T) {
		if (value < 5) {
			temp = 1;
		}
		if (value > SLIDER_WIDTH) {
			temp = 127;
		}
	}
	//printf("Slider value = %d, temp = %d\n", value, temp);
	return temp;
}

void XfsDrawSlider(Display *display, Window w, int x, int y, int width, int height, char *label, int activate) {
	XFontStruct *font;
	XTextItem ti[1];
	int state;
	int control_x, control_y;

	font = XLoadQueryFont(display, XFS_DEFAULT_FONT);
	ti[0].chars = label;
	ti[0].nchars = strlen(label);
	ti[0].delta = 0;
	ti[0].font = font->fid;

	control_x = XfsGetSliderControlPosX();
	control_y = XfsGetSliderControlPosY();

	if(control_x < SLIDER_CONTROL_WIDTH) {
		control_x = 0;
	}
	if (control_x > (SLIDER_WIDTH - SLIDER_CONTROL_WIDTH)) {
		control_x = (SLIDER_WIDTH - SLIDER_CONTROL_WIDTH);
	}
	// Clear the entire slider window
	XClearWindow(display, w);

	// Slider background base
	XDrawRectangle(display, w, gc_up, 0, 0, width, height);

	// Slider control
	XFillRectangle(display, w, gc_gray3, control_x, 0, SLIDER_CONTROL_WIDTH, SLIDER_CONTROL_HEIGHT);
	XDrawRectangle(display, w, gc, control_x, 0, SLIDER_CONTROL_WIDTH, SLIDER_CONTROL_HEIGHT);

	XUnloadFont(display, font->fid);
}

void XfsSliderEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data) {
	Window root_return;
	int x_return, y_return;
	unsigned int width_return, height_return;
	unsigned int border_width_return;
	unsigned int depth_return;

	XGetGeometry(display, e->xany.window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);

	*winret = e->xany.window;

	switch(e->type) {
		case MotionNotify:
			//printf("X = %d, Y = %d\n", e->xmotion.x, e->xmotion.y);
			XfsSetSliderControlPos(e->xmotion.x, e->xmotion.y);
			XfsDrawSlider(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			break;
		case ButtonPress:
			if (e->xbutton.button == 1) {
			}
			break;
		case ButtonRelease:
			if (e->xbutton.button == 1) {
			}
			break;
		case Expose:
			if (e->xexpose.count == 0) {
				XfsDrawSlider(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;

		default:
			*winret = 0;
	}
}

/***********************************************************************************************************/
/****************************************** LEVEL SETTING **************************************************/
/***********************************************************************************************************/

/* Create Operator Button Element */
Window XfsCreateLevelBox(Display *display, Window parent, int x, int y, int width, int height, char *label) {
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
	class_hint.res_class = XFS_LEVELBOX_CLASS;
	XSetClassHint(display, w, &class_hint);

	XSelectInput(display, w, ExposureMask | ButtonPressMask | ButtonReleaseMask | SubstructureNotifyMask);
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

int XfsGetLevelBoxState(Window window, int toggle) {
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

void XfsSetLevelBox(char *label, int level_value, int time_value) {
	switch(*label) {
		case 'a':
			amp_level_1 = level_value;
			time_level_1 = time_value;
			//printf("amp_level_1 = %d, time_level_1 = %d\n", amp_level_1, time_level_1);
			break;
		case 'b':
			amp_level_2 = level_value;
			time_level_2 = time_value;
			//printf("amp_level_2 = %d, time_level_2 = %d\n", amp_level_2, time_level_2);
			break;
		case 'c':
			amp_level_3 = level_value;
			time_level_3 = time_value;
			//printf("amp_level_3 = %d, time_level_3 = %d\n", amp_level_3, time_level_3);
			break;
		case 'd':
			amp_level_4 = level_value;
			time_level_4 = time_value;
			//printf("amp_level_4 = %d, time_level_4 = %d\n", amp_level_4, time_level_4);
			break;
		case 'e':
			amp_level_5 = level_value;
			time_level_5 = time_value;
			//printf("amp_level_5 = %d, time_level_5 = %d\n", amp_level_5, time_level_5);
			break;
		case 'f':
			amp_level_6 = level_value;
			time_level_6 = time_value;
			//printf("amp_level_6 = %d, time_level_6 = %d\n", amp_level_6, time_level_6);
			break;
		case 'g':
			amp_level_7 = level_value;
			time_level_7 = time_value;
			//printf("amp_level_7 = %d, time_level_7 = %d\n", amp_level_7, time_level_7);
			break;
	}
}

int XfsGetLevelAmplitude(char *label) {
	switch(*label) {
		case 'a':
			level_amplitude = amp_level_1;
			break;
		case 'b':
			level_amplitude = amp_level_2;
			break;
		case 'c':
			level_amplitude = amp_level_3;
			break;
		case 'd':
			level_amplitude = amp_level_4;
			break;
		case 'e':
			level_amplitude = amp_level_5;
			break;
		case 'f':
			level_amplitude = amp_level_6;
			break;
		case 'g':
			level_amplitude = amp_level_7;
			break;
	}
	return level_amplitude;
}

int XfsGetAmplitude(char *label) {
	switch(*label) {
		case 'a':
			return amp_level_1;
		case 'b':
			return amp_level_2;
		case 'c':
			return amp_level_3;
		case 'd':
			return amp_level_4;
		case 'e':
			return amp_level_5;
		case 'f':
			return amp_level_6;
		case 'g':
			return amp_level_7;
	}
}

int XfsGetLevelTimeInterval(char *label) {
	switch(*label) {
		case 'a':
			level_time_interval = time_level_1;
			break;
		case 'b':
			level_time_interval = time_level_2;
			break;
		case 'c':
			level_time_interval = time_level_3;
			break;
		case 'd':
			level_time_interval = time_level_4;
			break;
		case 'e':
			level_time_interval = time_level_5;
			break;
		case 'f':
			level_time_interval = time_level_6;
			break;
		case 'g':
			level_time_interval = time_level_7;
			break;
	}
	return level_time_interval;
}

int XfsGetLevelSet(char *label) {
	switch(*label) {
		case 'a':
			return 1;
		case 'b':
			return 2;
		case 'c':
			return 3;
		case 'd':
			return 4;
		case 'e':
			return 5;
		case 'f':
			return 6;
		case 'g':
			return 7;
	}
}

int XfsGetTimeInterval(char *label) {
	switch(*label) {
		case 'a':
			return time_level_1;
		case 'b':
			return time_level_2;
		case 'c':
			return time_level_3;
		case 'd':
			return time_level_4;
		case 'e':
			return time_level_5;
		case 'f':
			return time_level_6;
		case 'g':
			return time_level_7;
	}
}

char *XfsConvertToLevelValue(int value) {
	char *level_value = "";
	switch(value) {
		case 0:
			level_value = "0";
			break;
		case 1:
			level_value = "1";
			break;
		case 2:
			level_value = "2";
			break;
		case 3:
			level_value = "3";
			break;
		case 4:
			level_value = "4";
			break;
		case 5:
			level_value = "5";
			break;
		case 6:
			level_value = "6";
			break;
		case 7:
			level_value = "7";
			break;
		case 8:
			level_value = "8";
			break;
		case 9:
			level_value = "9";
			break;
		case 10:
			level_value = "10";
			break;
		case 11:
			level_value = "11";
			break;
		case 12:
			level_value = "12";
			break;
		case 13:
			level_value = "13";
			break;
		case 14:
			level_value = "14";
			break;
		case 15:
			level_value = "15";
			break;
		case 16:
			level_value = "16";
			break;
		case 17:
			level_value = "17";
			break;
		case 18:
			level_value = "18";
			break;
		case 19:
			level_value = "19";
			break;
		case 20:
			level_value = "20";
			break;
		case 21:
			level_value = "21";
			break;
		case 22:
			level_value = "22";
			break;
		case 23:
			level_value = "23";
			break;
		case 24:
			level_value = "24";
			break;
		case 25:
			level_value = "25";
			break;
		case 26:
			level_value = "26";
			break;
		case 27:
			level_value = "27";
			break;
		case 28:
			level_value = "28";
			break;
		case 29:
			level_value = "29";
			break;
		case 30:
			level_value = "30";
			break;
		case 31:
			level_value = "31";
			break;
		case 32:
			level_value = "32";
			break;
		case 33:
			level_value = "33";
			break;
		case 34:
			level_value = "34";
			break;
		case 35:
			level_value = "35";
			break;
		case 36:
			level_value = "36";
			break;
		case 37:
			level_value = "37";
			break;
		case 38:
			level_value = "38";
			break;
		case 39:
			level_value = "39";
			break;
		case 40:
			level_value = "40";
			break;
		case 41:
			level_value = "41";
			break;
		case 42:
			level_value = "42";
			break;
		case 43:
			level_value = "43";
			break;
		case 44:
			level_value = "44";
			break;
		case 45:
			level_value = "45";
			break;
		case 46:
			level_value = "46";
			break;
		case 47:
			level_value = "47";
			break;
		case 48:
			level_value = "48";
			break;
		case 49:
			level_value = "49";
			break;
		case 50:
			level_value = "50";
			break;
		case 51:
			level_value = "51";
			break;
		case 52:
			level_value = "52";
			break;
		case 53:
			level_value = "53";
			break;
		case 54:
			level_value = "54";
			break;
		case 55:
			level_value = "55";
			break;
		case 56:
			level_value = "56";
			break;
		case 57:
			level_value = "57";
			break;
		case 58:
			level_value = "58";
			break;
		case 59:
			level_value = "59";
			break;
		case 60:
			level_value = "60";
			break;
		case 61:
			level_value = "61";
			break;
		case 62:
			level_value = "62";
			break;
		case 63:
			level_value = "63";
			break;
		case 64:
			level_value = "64";
			break;
		case 65:
			level_value = "65";
			break;
		case 66:
			level_value = "66";
			break;
		case 67:
			level_value = "67";
			break;
		case 68:
			level_value = "68";
			break;
		case 69:
			level_value = "69";
			break;
		case 70:
			level_value = "70";
			break;
		case 71:
			level_value = "71";
			break;
		case 72:
			level_value = "72";
			break;
		case 73:
			level_value = "73";
			break;
		case 74:
			level_value = "74";
			break;
		case 75:
			level_value = "75";
			break;
		case 76:
			level_value = "76";
			break;
		case 77:
			level_value = "77";
			break;
		case 78:
			level_value = "78";
			break;
		case 79:
			level_value = "79";
			break;
		case 80:
			level_value = "80";
			break;
		case 81:
			level_value = "81";
			break;
		case 82:
			level_value = "82";
			break;
		case 83:
			level_value = "83";
			break;
		case 84:
			level_value = "84";
			break;
		case 85:
			level_value = "85";
			break;
		case 86:
			level_value = "86";
			break;
		case 87:
			level_value = "87";
			break;
		case 88:
			level_value = "88";
			break;
		case 89:
			level_value = "89";
			break;
		case 90:
			level_value = "90";
			break;
		case 91:
			level_value = "91";
			break;
		case 92:
			level_value = "92";
			break;
		case 93:
			level_value = "93";
			break;
		case 94:
			level_value = "94";
			break;
		case 95:
			level_value = "95";
			break;
		case 96:
			level_value = "96";
			break;
		case 97:
			level_value = "97";
			break;
		case 98:
			level_value = "98";
			break;
		case 99:
			level_value = "99";
			break;
		case 100:
			level_value = "100";
			break;
		case 101:
			level_value = "101";
			break;
		case 102:
			level_value = "102";
			break;
		case 103:
			level_value = "103";
			break;
		case 104:
			level_value = "104";
			break;
		case 105:
			level_value = "105";
			break;
		case 106:
			level_value = "106";
			break;
		case 107:
			level_value = "107";
			break;
		case 108:
			level_value = "108";
			break;
		case 109:
			level_value = "109";
			break;
		case 110:
			level_value = "110";
			break;
		case 111:
			level_value = "111";
			break;
		case 112:
			level_value = "112";
			break;
		case 113:
			level_value = "113";
			break;
		case 114:
			level_value = "114";
			break;
		case 115:
			level_value = "115";
			break;
		case 116:
			level_value = "116";
			break;
		case 117:
			level_value = "117";
			break;
		case 118:
			level_value = "118";
			break;
		case 119:
			level_value = "119";
			break;
		case 120:
			level_value = "120";
			break;
		case 121:
			level_value = "121";
			break;
		case 122:
			level_value = "122";
			break;
		case 123:
			level_value = "123";
			break;
		case 124:
			level_value = "124";
			break;
		case 125:
			level_value = "125";
			break;
		case 126:
			level_value = "126";
			break;
		case 127:
			level_value = "127";
			break;
		default:
			level_value = "0";
	}
	return level_value;
}

char *XfsConvertToTimeValue(int value) {
	char *time_value = "";
	switch(value) {
		case 1:
			time_value = "2.46ms";
			break;
		case 2:
			time_value = "2.5ms";
			break;
		case 3:
			time_value = "2.55ms";
			break;
		case 4:
			time_value = "2.6ms";
			break;
		case 5:
			time_value = "2.65ms";
			break;
		case 6:
			time_value = "2.71ms";
			break;
		case 7:
			time_value = "2.76ms";
			break;
		case 8:
			time_value = "2.82ms";
			break;
		case 9:
			time_value = "2.88ms";
			break;
		case 10:
			time_value = "2.94ms";
			break;
		case 11:
			time_value = "3.01ms";
			break;
		case 12:
			time_value = "3.08ms";
			break;
		case 13:
			time_value = "3.15ms";
			break;
		case 14:
			time_value = "3.23ms";
			break;
		case 15:
			time_value = "3.31ms";
			break;
		case 16:
			time_value = "3.39ms";
			break;
		case 17:
			time_value = "3.48ms";
			break;
		case 18:
			time_value = "3.57ms";
			break;
		case 19:
			time_value = "3.67ms";
			break;
		case 20:
			time_value = "3.77ms";
			break;
		case 21:
			time_value = "3.88ms";
			break;
		case 22:
			time_value = "3.99ms";
			break;
		case 23:
			time_value = "4.12ms";
			break;
		case 24:
			time_value = "4.25ms";
			break;
		case 25:
			time_value = "4.39ms";
			break;
		case 26:
			time_value = "4.53ms";
			break;
		case 27:
			time_value = "4.69ms";
			break;
		case 28:
			time_value = "4.86ms";
			break;
		case 29:
			time_value = "5.04ms";
			break;
		case 30:
			time_value = "5.24ms";
			break;
		case 31:
			time_value = "5.45ms";
			break;
		case 32:
			time_value = "5.69ms";
			break;
		case 33:
			time_value = "5.94ms";
			break;
		case 34:
			time_value = "6.21ms";
			break;
		case 35:
			time_value = "6.51ms";
			break;
		case 36:
			time_value = "6.85ms";
			break;
		case 37:
			time_value = "7.21ms";
			break;
		case 38:
			time_value = "10.07ms";
			break;
		case 39:
			time_value = "10.6ms";
			break;
		case 40:
			time_value = "11.18ms";
			break;
		case 41:
			time_value = "11.84ms";
			break;
		case 42:
			time_value = "12ms";
			break;
		case 43:
			time_value = "13ms";
			break;
		case 44:
			time_value = "14ms";
			break;
		case 45:
			time_value = "15ms";
			break;
		case 46:
			time_value = "16ms";
			break;
		case 47:
			time_value = "18ms";
			break;
		case 48:
			time_value = "20ms";
			break;
		case 49:
			time_value = "22ms";
			break;
		case 50:
			time_value = "25ms";
			break;
		case 51:
			time_value = "28ms";
			break;
		case 52:
			time_value = "33ms";
			break;
		case 53:
			time_value = "40ms";
			break;
		case 54:
			time_value = "50ms";
			break;
		case 55:
			time_value = "67ms";
			break;
		case 56:
			time_value = "100ms";
			break;
		case 57:
			time_value = "103ms";
			break;
		case 58:
			time_value = "105ms";
			break;
		case 59:
			time_value = "108ms";
			break;
		case 60:
			time_value = "111ms";
			break;
		case 61:
			time_value = "115ms";
			break;
		case 62:
			time_value = "118ms";
			break;
		case 63:
			time_value = "122ms";
			break;
		case 64:
			time_value = "125ms";
			break;
		case 65:
			time_value = "129ms";
			break;
		case 66:
			time_value = "134ms";
			break;
		case 67:
			time_value = "138ms";
			break;
		case 68:
			time_value = "143ms";
			break;
		case 69:
			time_value = "149ms";
			break;
		case 70:
			time_value = "154ms";
			break;
		case 71:
			time_value = "161ms";
			break;
		case 72:
			time_value = "167ms";
			break;
		case 73:
			time_value = "175ms";
			break;
		case 74:
			time_value = "183ms";
			break;
		case 75:
			time_value = "191ms";
			break;
		case 76:
			time_value = "201ms";
			break;
		case 77:
			time_value = "211ms";
			break;
		case 78:
			time_value = "223ms";
			break;
		case 79:
			time_value = "236ms";
			break;
		case 80:
			time_value = "251ms";
			break;
		case 81:
			time_value = "268ms";
			break;
		case 82:
			time_value = "287ms";
			break;
		case 83:
			time_value = "309ms";
			break;
		case 84:
			time_value = "335ms";
			break;
		case 85:
			time_value = "366ms";
			break;
		case 86:
			time_value = "402ms";
			break;
		case 87:
			time_value = "447ms";
			break;
		case 88:
			time_value = "503ms";
			break;
		case 89:
			time_value = "516ms";
			break;
		case 90:
			time_value = "529ms";
			break;
		case 91:
			time_value = "544ms";
			break;
		case 92:
			time_value = "559ms";
			break;
		case 93:
			time_value = "575ms";
			break;
		case 94:
			time_value = "592ms";
			break;
		case 95:
			time_value = "610ms";
			break;
		case 96:
			time_value = "629ms";
			break;
		case 97:
			time_value = "649ms";
			break;
		case 98:
			time_value = "671ms";
			break;
		case 99:
			time_value = "694ms";
			break;
		case 100:
			time_value = "719ms";
			break;
		case 101:
			time_value = "745ms";
			break;
		case 102:
			time_value = "774ms";
			break;
		case 103:
			time_value = "805ms";
			break;
		case 104:
			time_value = "838ms";
			break;
		case 105:
			time_value = "875ms";
			break;
		case 106:
			time_value = "915ms";
			break;
		case 107:
			time_value = "958ms";
			break;
		//from nearly 1s to 20s
		case 108:
			time_value = "1.006s";
			break;
		case 109:
			time_value = "1.059s";
			break;
		case 110:
			time_value = "1.118s";
			break;
		case 111:
			time_value = "1.184s";
			break;
		case 112:
			time_value = "1.258s";
			break;
		case 113:
			time_value = "1.342s";
			break;
		case 114:
			time_value = "1.438s";
			break;
		case 115:
			time_value = "1.548s";
			break;
		case 116:
			time_value = "1.677s";
			break;
		case 117:
			time_value = "1.83s";
			break;
		case 118:
			time_value = "2.013s";
			break;
		case 119:
			time_value = "2.236s";
			break;
		case 120:
			time_value = "2.516s";
			break;
		case 121:
			time_value = "2.876s";
			break;
		case 122:
			time_value = "3.355s";
			break;
		case 123:
			time_value = "4.026s";
			break;
		case 124:
			time_value = "5.033s";
			break;
		case 125:
			time_value = "6.71s";
			break;
		case 126:
			time_value = "10.066s";
			break;
		case 127:
			time_value = "20.132s";
			break;
		default:
			time_value = "2.46ms";
	}
	return time_value;
}

char *XfsGetLevelStatus(char *level, char *level_value, char *time_stamp, char *time_value) {
	char *seperator = ", ";
	// +1 for the zero-terminator;
	int status_size = strlen(level) + strlen(level_value) + strlen(seperator) + strlen(time_stamp) + strlen(time_value);
	char *status = (char *)malloc(status_size + 1);
	strcpy(status, level);
	strcat(status, level_value);
	strcat(status, seperator);
	strcat(status, time_stamp);
	strcat(status, time_value);
	return status;
}

char *XfsUpdateLevelBox(char *label) {
	char *temp;
	switch(*label) {
		case 'a':
			temp = (char *)XfsGetLevelStatus("L1:", (char *)XfsConvertToLevelValue(XfsGetLevelAmplitude(label)), "HT:", (char *)XfsConvertToTimeValue(XfsGetLevelTimeInterval(label)));
			break;
		case 'b':
			temp = (char *)XfsGetLevelStatus("L2:", (char *)XfsConvertToLevelValue(XfsGetLevelAmplitude(label)), "T1:", (char *)XfsConvertToTimeValue(XfsGetLevelTimeInterval(label)));
			break;
		case 'c':
			temp = (char *)XfsGetLevelStatus("L3:", (char *)XfsConvertToLevelValue(XfsGetLevelAmplitude(label)), "T2:", (char *)XfsConvertToTimeValue(XfsGetLevelTimeInterval(label)));
			break;
		case 'd':
			temp = (char *)XfsGetLevelStatus("L4:", (char *)XfsConvertToLevelValue(XfsGetLevelAmplitude(label)), "T3:", (char *)XfsConvertToTimeValue(XfsGetLevelTimeInterval(label)));
			break;
		case 'e':
			temp = (char *)XfsGetLevelStatus("L5:", (char *)XfsConvertToLevelValue(XfsGetLevelAmplitude(label)), "T4:", (char *)XfsConvertToTimeValue(XfsGetLevelTimeInterval(label)));
			break;
		case 'f':
			temp = (char *)XfsGetLevelStatus("L6:", (char *)XfsConvertToLevelValue(XfsGetLevelAmplitude(label)), "T5:", (char *)XfsConvertToTimeValue(XfsGetLevelTimeInterval(label)));
			break;
		case 'g':
			temp = (char *)XfsGetLevelStatus("L7:", (char *)XfsConvertToLevelValue(XfsGetLevelAmplitude(label)), "T6:", (char *)XfsConvertToTimeValue(XfsGetLevelTimeInterval(label)));
			break;
	}
	return temp;
}

void XfsDrawLevelBoxButton(Display *display, Window w, int x, int y, int width, int height, char *label, int activate) {
	XFontStruct *font;
	XTextItem ti[1];

	font = XLoadQueryFont(display, XFS_DEFAULT_FONT);
	ti[0].chars = (char *)XfsUpdateLevelBox(label); //label;
	ti[0].nchars = strlen((char *)XfsUpdateLevelBox(label));
	ti[0].delta = 0;
	ti[0].font = font->fid;

	XClearWindow(display, w);
	XfsDrawRectShadow(display, w, activate?gc_down:gc_up, 0, 0, width, height);

	//XFillRectangle(display, w, gc_gray3, LB_X, LB_Y, LB_WIDTH, LB_HEIGHT);
	//XDrawRectangle(display, w, gc, LB_X, LB_Y, LB_WIDTH, LB_HEIGHT);
	XDrawText(display, w, gc, LB_X * 2 + LB_WIDTH, (height - (font->ascent + font->descent)) / 2 + font->ascent, ti, 1);

	XUnloadFont(display, font->fid);
}

void XfsLevelBoxEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data) {
	Window root_return;
	int x_return, y_return;
	unsigned int width_return, height_return;
	unsigned int border_width_return;
	unsigned int depth_return;
	bool pressed = true;

	XGetGeometry(display, e->xany.window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);

	*winret = e->xany.window;

	switch(e->type) {
		case ButtonPress:
			if (e->xbutton.button == 1) {
				XfsDrawLevelBoxButton(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 1);
			}
			break;
		case ButtonRelease:
			if (e->xbutton.button == 1) {
				XfsDrawLevelBoxButton(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;
		case Expose:
			if (e->xexpose.count == 0) {
				XfsDrawLevelBoxButton(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;

		default:
			*winret = 0;
	}
}

/***********************************************************************************************************/
/************************************* OPERATOR SLIDER SETTING *********************************************/
/***********************************************************************************************************/

/* Create Operator Slider Element */
Window XfsCreateOperatorSlider(Display *display, Window parent, int x, int y, int width, int height, char *label) {
	Window w;
	XClassHint class_hint;
	XGCValues gcv;
	XColor color_up, color_down, color_gray3;

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

	}

	w = XCreateSimpleWindow(display, parent, x, y, width, height, 1, blackpixel, color_up.pixel);
	class_hint.res_name = label;
	class_hint.res_class = XFS_OPERATOR_SLIDER_CLASS;
	XSetClassHint(display, w, &class_hint);

	XSelectInput(display, w, ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask);
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

void XfsSetOperatorSliderControlPos(int x, int y) {
	operator_slider_control_x = x;
	operator_slider_control_y = y;
}

int XfsGetOperatorSliderControlPosX() {
	return operator_slider_control_x;
}

int XfsGetOperatorSliderControlPosY() {
	return operator_slider_control_y;
}

int XfsCalculateOperatorSlider(int type, int value) {
	int temp;
	switch(type) {
		case TYPE_COEF:
			temp = value / 50;
			if (temp < 0) {
				temp = 0;
			}
			if (temp > 4) {
				temp = 4;
			}
			operator_coef = temp;
			return operator_coef;

		case TYPE_ARR:
			temp = (int)(value / 4.4);
			if (temp < 1) {
				temp = 1;
			}
			if (temp > 45) {
				temp = 45;
			}
			operator_arr = temp;
			return operator_arr;
	}
}

int XfsGetModulationCoefficient() {
	return operator_coef;
}

int XfsGetArrangementOption() {
	return operator_arr;
}

void XfsDrawOperatorSlider(Display *display, Window w, int x, int y, int width, int height, char *label, int activate) {
	XFontStruct *font;
	XTextItem ti[1];
	int state;
	int control_x, control_y;

	font = XLoadQueryFont(display, XFS_DEFAULT_FONT);
	ti[0].chars = label;
	ti[0].nchars = strlen(label);
	ti[0].delta = 0;
	ti[0].font = font->fid;

	control_x = XfsGetOperatorSliderControlPosX();
	control_y = XfsGetOperatorSliderControlPosY();

	if(control_y < 0) {
		control_y = 0;
	}
	if (control_y > (OPERATOR_SLIDER_HEIGHT - OPERATOR_SLIDER_CONTROL_HEIGHT)) {
		control_y = (OPERATOR_SLIDER_HEIGHT - OPERATOR_SLIDER_CONTROL_HEIGHT);
	}
	// Clear the entire slider window
	XClearWindow(display, w);

	// Slider background base
	XDrawRectangle(display, w, gc_up, 0, 0, width, height);

	// Slider control
	XFillRectangle(display, w, gc_gray3, 0, control_y, OPERATOR_SLIDER_CONTROL_WIDTH, OPERATOR_SLIDER_CONTROL_HEIGHT);
	XDrawRectangle(display, w, gc, 0, control_y, OPERATOR_SLIDER_CONTROL_WIDTH, OPERATOR_SLIDER_CONTROL_HEIGHT);

	XUnloadFont(display, font->fid);
}

void XfsOperatorSliderEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data) {
	Window root_return;
	int x_return, y_return;
	unsigned int width_return, height_return;
	unsigned int border_width_return;
	unsigned int depth_return;

	XGetGeometry(display, e->xany.window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);

	*winret = e->xany.window;

	switch(e->type) {
		case MotionNotify:
			//printf("X = %d, Y = %d\n", e->xmotion.x, e->xmotion.y);
			XfsSetOperatorSliderControlPos(e->xmotion.x, e->xmotion.y);
			XfsDrawOperatorSlider(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			break;
		case ButtonPress:
			if (e->xbutton.button == 1) {
			}
			break;
		case ButtonRelease:
			if (e->xbutton.button == 1) {
			}
			break;
		case Expose:
			if (e->xexpose.count == 0) {
				XfsDrawOperatorSlider(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;

		default:
			*winret = 0;
	}
}

/***********************************************************************************************************/
/*************************************** LEVEL DRAWING SETTING *********************************************/
/***********************************************************************************************************/

/* Create Level Drawing Element */
Window XfsCreateLevelDrawing(Display *display, Window parent, int x, int y, int width, int height, char *label) {
	Window w;
	XClassHint class_hint;
	XGCValues gcv;
	XColor color_up, color_down, color_gray3;

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

	}

	w = XCreateSimpleWindow(display, parent, x, y, width, height, 1, blackpixel, color_up.pixel);
	class_hint.res_name = label;
	class_hint.res_class = XFS_LEVELDRAWING_CLASS;
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

int XfsConvertToXAxis(int time_index) {
	int temp;
	if (time_index < 107) {
		if (time_index < 11) {
			temp = 7;
		}
		if (time_index > 10 && time_index < 26) {
			temp = 15;
		}
		if (time_index > 25 && time_index < 41) {
			temp = 23;
		}
		if (time_index > 40 && time_index < 56) {
			temp = 31;
		}
		if (time_index > 55 && time_index < 71) {
			temp = 39;
		}
		if (time_index > 70 && time_index < 86) {
			temp = 47;
		}
		if (time_index > 85) {
			temp = 55;
		}

	}
	else if (time_index > 107) {
		int diff = (time_index - 107) / 4;
		temp = 55 + diff;
	}
}

void XfsDrawLevelDrawing(Display *display, Window w, int x, int y, int width, int height, char *label, int activate) {
	XFontStruct *font;
	XTextItem ti[1];
	int y_inverted = XFS_DRAWING_HEIGHT;
	int x_temp_1, x_temp_2, x_temp_3, x_temp_4, x_temp_5, x_temp_6, x_temp_7, x_temp_8, x_temp_9, x_temp_10;
	int y_temp_1, y_temp_2, y_temp_3, y_temp_4, y_temp_5, y_temp_6, y_temp_7, y_temp_8, y_temp_9, y_temp_10;

	font = XLoadQueryFont(display, XFS_DEFAULT_FONT);
	ti[0].chars = label;
	ti[0].nchars = strlen(label);
	ti[0].delta = 0;
	ti[0].font = font->fid;

	// Clear the entire slider window
	XClearWindow(display, w);

	if (XfsGetMode() == TYPE_ONESHOT) {
		// Original - Level 1
		x_temp_1 = XfsConvertToXAxis(XfsGetTimeInterval("a"));
		y_temp_1 = y_inverted - XfsGetAmplitude("a");
		XDrawLine(display, w, gc, 0, y_temp_1, x_temp_1, y_temp_1);

		// Level 1 - Level 2
		x_temp_2 = x_temp_1 + XfsConvertToXAxis(XfsGetTimeInterval("b"));
		y_temp_2 = y_inverted - XfsGetAmplitude("b");
		XDrawLine(display, w, gc, x_temp_1, y_temp_1, x_temp_2, y_temp_2);

		// Level 2 - Level 3
		x_temp_3 = x_temp_2 + XfsConvertToXAxis(XfsGetTimeInterval("c"));
		y_temp_3 = y_inverted - XfsGetAmplitude("c");
		XDrawLine(display, w, gc, x_temp_2, y_temp_2, x_temp_3, y_temp_3);

		// Level 3 - Level 4
		x_temp_4 = x_temp_3 + XfsConvertToXAxis(XfsGetTimeInterval("d"));
		y_temp_4 = y_inverted - XfsGetAmplitude("d");
		XDrawLine(display, w, gc, x_temp_3, y_temp_3, x_temp_4, y_temp_4);

		// Level 4 - Level 5
		x_temp_5 = x_temp_4 + XfsConvertToXAxis(XfsGetTimeInterval("e"));
		y_temp_5 = y_inverted - XfsGetAmplitude("e");
		XDrawLine(display, w, gc, x_temp_4, y_temp_4, x_temp_5, y_temp_5);

		// Level 5
		x_temp_6 = x_temp_5 + 60;
		y_temp_6 = y_temp_5;
		XDrawLine(display, w, gc, x_temp_5, y_temp_5, x_temp_6, y_temp_6);

		// Level 5 - Level 6
		x_temp_7 = x_temp_6 + XfsConvertToXAxis(XfsGetTimeInterval("f"));
		y_temp_7 = y_inverted - XfsGetAmplitude("f");
		XDrawLine(display, w, gc, x_temp_6, y_temp_6, x_temp_7, y_temp_7);

		// Level 6 - Level 7
		x_temp_8 = x_temp_7 + XfsConvertToXAxis(XfsGetTimeInterval("g"));
		y_temp_8 = y_inverted - XfsGetAmplitude("g");
		XDrawLine(display, w, gc, x_temp_7, y_temp_7, x_temp_8, y_temp_8);
	}

	if (XfsGetMode() == TYPE_LOOP) {
		// Original - Level 1
		x_temp_1 = XfsConvertToXAxis(XfsGetTimeInterval("a"));
		y_temp_1 = y_inverted - XfsGetAmplitude("a");
		XDrawLine(display, w, gc, 0, y_temp_1, x_temp_1, y_temp_1);

		// Level 1 - Level 2
		x_temp_2 = x_temp_1 + XfsConvertToXAxis(XfsGetTimeInterval("b"));
		y_temp_2 = y_inverted - XfsGetAmplitude("b");
		XDrawLine(display, w, gc, x_temp_1, y_temp_1, x_temp_2, y_temp_2);

		// Level 2 - Level 3
		x_temp_3 = x_temp_2 + XfsConvertToXAxis(XfsGetTimeInterval("c"));
		y_temp_3 = y_inverted - XfsGetAmplitude("c");
		XDrawLine(display, w, gc, x_temp_2, y_temp_2, x_temp_3, y_temp_3);

		// Level 3 - Level 4
		x_temp_4 = x_temp_3 + XfsConvertToXAxis(XfsGetTimeInterval("d"));
		y_temp_4 = y_inverted - XfsGetAmplitude("d");
		XDrawLine(display, w, gc, x_temp_3, y_temp_3, x_temp_4, y_temp_4);

		// Level 4 - Level 5
		x_temp_5 = x_temp_4 + XfsConvertToXAxis(XfsGetTimeInterval("e"));
		y_temp_5 = y_inverted - XfsGetAmplitude("e");
		XDrawLine(display, w, gc, x_temp_4, y_temp_4, x_temp_5, y_temp_5);

		// Level 5 - Level 4
		x_temp_6 = x_temp_5 + 20;
		XDrawLine(display, w, gc, x_temp_5, y_temp_5, x_temp_6, y_temp_4);

		// Level 4 - Level 5
		x_temp_7 = x_temp_6 + 20;
		XDrawLine(display, w, gc, x_temp_6, y_temp_4, x_temp_7, y_temp_5);

		// Level 5 - Level 4
		x_temp_8 = x_temp_7 + 20;
		XDrawLine(display, w, gc, x_temp_7, y_temp_5, x_temp_8, y_temp_4);

		// Level 4 - Level 6
		x_temp_9 = x_temp_8 + XfsConvertToXAxis(XfsGetTimeInterval("f"));
		y_temp_9 = y_inverted - XfsGetAmplitude("f");
		XDrawLine(display, w, gc, x_temp_8, y_temp_4, x_temp_9, y_temp_9);

		// Level 6 - Level 7
		x_temp_10 = x_temp_9 + XfsConvertToXAxis(XfsGetTimeInterval("g"));
		y_temp_10 = y_inverted - XfsGetAmplitude("g");
		XDrawLine(display, w, gc, x_temp_9, y_temp_9, x_temp_10, y_temp_10);
	}

	XUnloadFont(display, font->fid);
}

void XfsLevelDrawingEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data) {
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
				XfsDrawLevelDrawing(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;
		case ButtonRelease:
			if (e->xbutton.button == 1) {
				XfsDrawLevelDrawing(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;
		case Expose:
			if (e->xexpose.count == 0) {
				XfsDrawLevelDrawing(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;

		default:
			*winret = 0;
	}
}

/***********************************************************************************************************/
/****************************************** FM DISPLAY SETTING *********************************************/
/***********************************************************************************************************/

/* Create FM Display Element */
Window XfsCreateFMDisplay(Display *display, Window parent, int x, int y, int width, int height, char *label) {
	Window w;
	XClassHint class_hint;
	XGCValues gcv;
	XColor color_up, color_down, color_gray3;

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

	}

	w = XCreateSimpleWindow(display, parent, x, y, width, height, 1, blackpixel, color_up.pixel);
	class_hint.res_name = label;
	class_hint.res_class = XFS_FMDISPLAY_CLASS;
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

void XfsDrawBlock(Display *display, Window w, int x, int y, int width, int height, char *number) {
	XFontStruct *font;
	XTextItem ti[1];

	font = XLoadQueryFont(display, XFS_DEFAULT_FONT);
	ti[0].chars = number;
	ti[0].nchars = strlen(number);
	ti[0].delta = 0;
	ti[0].font = font->fid;

	XDrawRectangle(display, w, gc, x, y, width, height);
	XDrawText(display, w, gc, x + 15, y + 15, ti, 1);

	XUnloadFont(display, font->fid);
}

void XfsDrawFMDisplay(Display *display, Window w, int x, int y, int width, int height, char *label, int activate) {
	XFontStruct *font;
	XTextItem ti[1];

	label = (char *)XfsConvertToLevelValue(XfsGetArrangementOption());

	font = XLoadQueryFont(display, XFS_DEFAULT_FONT);
	ti[0].chars = label;
	ti[0].nchars = strlen(label);
	ti[0].delta = 0;
	ti[0].font = font->fid;

	// Clear the entire slider window
	XClearWindow(display, w);

	if (XfsGetArrangementOption() == 1) {
		int x_6 = 80;
		int x_5 = 80;
		int x_4 = 80;
		int x_3 = 80;
		int x_2 = 80;
		int x_1 = 80;
		int y_6 = 20;
		int y_5 = 20 + 1 * 35;
		int y_4 = 20 + 2 * 35;
		int y_3 = 20 + 3 * 35;
		int y_2 = 20 + 4 * 35;
		int y_1 = 20 + 5 * 35;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 2) {
		int x_6 = 70 + 1 * 50;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70;
		int y_6 = 20;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 3 * 40;
		int y_1 = 20 + 4 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 3) {
		int x_6 = 70 + 50;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70;
		int y_6 = 20 + 1 * 40;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 3 * 40;
		int y_1 = 20 + 4 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 4) {
		int x_6 = 70 + 1 * 50;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70;
		int y_6 = 20 + 2 * 40;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 3 * 40;
		int y_1 = 20 + 4 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 5) {
		int x_6 = 70 + 1 * 50;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70;
		int y_6 = 20 + 3 * 40;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 3 * 40;
		int y_1 = 20 + 4 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 6) {
		int x_6 = 35 + 2 * 50;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35;
		int x_3 = 35;
		int x_2 = 35;
		int x_1 = 35;
		int y_6 = 20;
		int y_5 = 20;
		int y_4 = 20;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 7) {
		int x_6 = 70 + 50;
		int x_5 = 70 + 50;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70;
		int y_6 = 20;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 8) {
		int x_6 = 35 + 2 * 50;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35;
		int x_3 = 35;
		int x_2 = 35;
		int x_1 = 35;
		int y_6 = 20 + 1 * 40;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 9) {
		int x_6 = 35;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35 + 1 * 50;
		int x_3 = 35 + 2 * 50;
		int x_2 = 35 + 1 * 50;
		int x_1 = 35 + 1 * 50;
		int y_6 = 20;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 10) {
		int x_6 = 70 + 1 * 60;
		int x_5 = 70 + 1 * 60;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70;
		int y_6 = 20 + 1 * 40;
		int y_5 = 20 + 2 * 40;
		int y_4 = 20;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 11) {
		int x_6 = 35 + 2 * 50;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35;
		int x_3 = 35;
		int x_2 = 35;
		int x_1 = 35;
		int y_6 = 20 + 2 * 40;
		int y_5 = 20 + 2 * 40;
		int y_4 = 20;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 12) {
		int x_6 = 70 + 1 * 50;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70 + 1 * 50;
		int x_1 = 70;
		int y_6 = 20;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 13) {
		int x_6 = 35;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35 + 1 * 50;
		int x_3 = 35 + 1 * 50;
		int x_2 = 35 + 2 * 50;
		int x_1 = 35 + 1 * 50;
		int y_6 = 20 + 1 * 40;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 14) {
		int x_6 = 10 + 3 * 50;
		int x_5 = 10 + 2 * 50;
		int x_4 = 10 + 1 * 50;
		int x_3 = 10;
		int x_2 = 10;
		int x_1 = 10;
		int y_6 = 50;
		int y_5 = 50;
		int y_4 = 50;
		int y_3 = 50;
		int y_2 = 50 + 1 * 40;
		int y_1 = 50 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 15) {
		int x_6 = 10 + 3 * 50;
		int x_5 = 10 + 2 * 50;
		int x_4 = 10 + 50;
		int x_3 = 10 + 50;
		int x_2 = 10;
		int x_1 = 10;
		int y_6 = 50;
		int y_5 = 50;
		int y_4 = 50;
		int y_3 = 50 + 1 * 40;
		int y_2 = 50 + 1 * 40;
		int y_1 = 50 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 16) {
		int x_6 = 35 + 2 * 50;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35 + 1 * 50;
		int x_3 = 35;
		int x_2 = 35;
		int x_1 = 35;
		int y_6 = 50 + 1 * 40;
		int y_5 = 50;
		int y_4 = 50 + 1 * 40;
		int y_3 = 50;
		int y_2 = 50 + 1 * 40;
		int y_1 = 50 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 17) {
		int x_6 = 35 + 2 * 50;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35 + 1 * 50;
		int x_3 = 35;
		int x_2 = 35;
		int x_1 = 35;
		int y_6 = 50;
		int y_5 = 50;
		int y_4 = 50 + 1 * 40;
		int y_3 = 50 + 8;
		int y_2 = 50 + 1 * 40;
		int y_1 = 50 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 18) {
		int x_6 = 10 + 3 * 50;
		int x_5 = 10 + 2 * 50;
		int x_4 = 10 + 1 * 50;
		int x_3 = 10;
		int x_2 = 10;
		int x_1 = 10;
		int y_6 = 50 + 1 * 40;
		int y_5 = 50 + 1 * 40;
		int y_4 = 50 + 1 * 40;
		int y_3 = 50;
		int y_2 = 50 + 1 * 40;
		int y_1 = 50 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 19) {
		int x_6 = 35 + 1 * 50;
		int x_5 = 35 + 2 * 50;
		int x_4 = 35 + 1 * 50;
		int x_3 = 35;
		int x_2 = 35;
		int x_1 = 35;
		int y_6 = 50;
		int y_5 = 50 + 1 * 40;
		int y_4 = 50 + 1 * 40;
		int y_3 = 50;
		int y_2 = 50 + 1 * 40;
		int y_1 = 50 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 20) {
		int x_6 = 10 + 4 * 35;
		int x_5 = 10 + 3 * 35;
		int x_4 = 10 + 2 * 35;
		int x_3 = 10 + 1 * 35;
		int x_2 = 10;
		int x_1 = 10;
		int y_6 = 80;
		int y_5 = 80;
		int y_4 = 80;
		int y_3 = 80;
		int y_2 = 80;
		int y_1 = 80 + 1 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 21) {
		int x_6 = 70;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70 + 1 * 50;
		int y_6 = 20;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20 + 2 * 40;
		int y_3 = 20 + 3 * 40;
		int y_2 = 20 + 4 * 40;
		int y_1 = 20 + 4 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 22) {
		int x_6 = 70 + 1 * 50;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70 + 1 * 50;
		int y_6 = 20;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 3 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 23) {
		int x_6 = 70 + 1 * 50;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70 + 1 * 50;
		int y_6 = 20 + 1 * 40;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 3 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 24) {
		int x_6 = 70 + 1 * 50;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70 + 1 * 50;
		int y_6 = 20 + 2 * 40;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 3 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 25) {
		int x_6 = 35 + 2 * 50;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35;
		int x_3 = 35;
		int x_2 = 35;
		int x_1 = 35 + 1 * 50;
		int y_6 = 20;
		int y_5 = 20;
		int y_4 = 20;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 26) {
		int x_6 = 70 + 1 * 50;
		int x_5 = 70 + 1 * 50;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70;
		int x_1 = 70 + 1 * 50;
		int y_6 = 20;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 45;
		int y_1 = 20 + 2 * 45;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 27) {
		int x_6 = 35 + 2 * 50;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35;
		int x_3 = 35;
		int x_2 = 35;
		int x_1 = 35 + 1 * 50;
		int y_6 = 20 + 1 * 40;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 45;
		int y_1 = 20 + 2 * 45;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 28) {
		int x_6 = 35;
		int x_5 = 35 + 1 * 50;
		int x_4 = 35 + 1 * 50;
		int x_3 = 35 + 2 * 50;
		int x_2 = 35 + 1 * 50;
		int x_1 = 35 + 2 * 50;
		int y_6 = 20;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 29) {
		int x_6 = 10 + 3 * 45;
		int x_5 = 10 + 2 * 45;
		int x_4 = 10 + 1 * 45;
		int x_3 = 10;
		int x_2 = 10;
		int x_1 = 10 + 1 * 45;
		int y_6 = 50;
		int y_5 = 50;
		int y_4 = 50;
		int y_3 = 50;
		int y_2 = 50 + 1 * 40;
		int y_1 = 50 + 1 * 40 ;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 30) {
		int x_6 = 70;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70 + 1 * 50;
		int x_1 = 70 + 1 * 50;
		int y_6 = 20;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20 + 2 * 40;
		int y_3 = 20 + 3 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 31) {
		int x_6 = 70 + 1 * 50;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70;
		int x_2 = 70 + 1 * 50;
		int x_1 = 70 + 1 * 50;
		int y_6 = 20;
		int y_5 = 20;
		int y_4 = 20 + 1 * 35;
		int y_3 = 20 + 2 * 35;
		int y_2 = 20 + 1 * 35;
		int y_1 = 20 + 2 * 35;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 32) {
		int x_6 = 35 + 1 * 50;
		int x_5 = 35;
		int x_4 = 35;
		int x_3 = 35;
		int x_2 = 35 + 2 * 50;
		int x_1 = 35 + 2 * 50;
		int y_6 = 20 + 1 * 40;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 1 * 40 + 8;
		int y_1 = 20 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 33) {
		int x_6 = 10 + 2 * 45;
		int x_5 = 10 + 1 * 45;
		int x_4 = 10;
		int x_3 = 10;
		int x_2 = 10 + 3 * 45;
		int x_1 = 10 + 3 * 45;
		int y_6 = 50;
		int y_5 = 50;
		int y_4 = 50;
		int y_3 = 50 + 1 * 40;
		int y_2 = 50 + 8;
		int y_1 = 50 + 1 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 34) {
		int x_6 = 70;
		int x_5 = 70;
		int x_4 = 70;
		int x_3 = 70 + 1 * 50;
		int x_2 = 70 + 1 * 50;
		int x_1 = 70 + 1 * 50;
		int y_6 = 20;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20 + 2 * 40;
		int y_3 = 20;
		int y_2 = 20 + 1 * 40;
		int y_1 = 20 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 35) {
		int x_6 = 35 + 1 * 50;
		int x_5 = 35;
		int x_4 = 35;
		int x_3 = 35 + 2 * 50;
		int x_2 = 35 + 2 * 50;
		int x_1 = 35 + 2 * 50;
		int y_6 = 20 + 10;
		int y_5 = 20 + 10;
		int y_4 = 20 + 2 * 40;
		int y_3 = 20;
		int y_2 = 20 + 1 * 40;
		int y_1 = 20 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 36) {
		int x_6 = 10 + 1 * 45;
		int x_5 = 10;
		int x_4 = 10;
		int x_3 = 10 + 3 * 45;
		int x_2 = 10 + 2 * 45;
		int x_1 = 10 + 2 * 45;
		int y_6 = 50;
		int y_5 = 50;
		int y_4 = 50 + 1 * 50;
		int y_3 = 50;
		int y_2 = 50;
		int y_1 = 50 + 1 * 50;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 37) {
		int x_6 = 35;
		int x_5 = 35;
		int x_4 = 35;
		int x_3 = 35;
		int x_2 = 35 + 1 * 50;
		int x_1 = 35 + 2 * 50;
		int y_6 = 20;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20 + 2 * 40;
		int y_3 = 20 + 3 * 40;
		int y_2 = 20 + 3 * 40;
		int y_1 = 20 + 3 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 38) {
		int x_6 = 35 + 1 * 50;
		int x_5 = 35;
		int x_4 = 35;
		int x_3 = 35;
		int x_2 = 35 + 1 * 50;
		int x_1 = 35 + 2 * 50;
		int y_6 = 20;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 39) {
		int x_6 = 35 + 1 * 50;
		int x_5 = 35;
		int x_4 = 35;
		int x_3 = 35;
		int x_2 = 35 + 1 * 50;
		int x_1 = 35 + 2 * 50;
		int y_6 = 20 + 1 * 40;
		int y_5 = 20;
		int y_4 = 20 + 1 * 40;
		int y_3 = 20 + 2 * 40;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 40) {
		int x_6 = 35;
		int x_5 = 35;
		int x_4 = 35;
		int x_3 = 35 + 1 * 50;
		int x_2 = 35 + 1 * 50;
		int x_1 = 35 + 2 * 50;
		int y_6 = 20;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20 + 2 * 40;
		int y_3 = 20 + 10;
		int y_2 = 20 + 2 * 40;
		int y_1 = 20 + 2 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 41) {
		int x_6 = 10 + 1 * 45;
		int x_5 = 10;
		int x_4 = 10;
		int x_3 = 10 + 2 * 45;
		int x_2 = 10 + 2 * 45;
		int x_1 = 10 + 3 * 45;
		int y_6 = 50;
		int y_5 = 50;
		int y_4 = 50 + 1 * 40;
		int y_3 = 50;
		int y_2 = 50 + 1 * 40;
		int y_1 = 50 + 1 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 42) {
		int x_6 = 35;
		int x_5 = 35;
		int x_4 = 35 + 1 * 50;
		int x_3 = 35 + 1 * 50;
		int x_2 = 35 + 2 * 50;
		int x_1 = 35 + 2 * 50;
		int y_6 = 20;
		int y_5 = 20 + 1 * 40;
		int y_4 = 20;
		int y_3 = 20 + 1 * 40;
		int y_2 = 20;
		int y_1 = 20 + 1 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}
	else if (XfsGetArrangementOption() == 43) {
		int x_6 = 10;
		int x_5 = 10;
		int x_4 = 10;
		int x_3 = 10 + 1 * 45;
		int x_2 = 10 + 2 * 45;
		int x_1 = 10 + 3 * 45;
		int y_6 = 50;
		int y_5 = 50 + 1 * 50;
		int y_4 = 50 + 2 * 50;
		int y_3 = 50 + 2 * 50;
		int y_2 = 50 + 2 * 50;
		int y_1 = 50 + 2 * 50;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 44) {
		int x_6 = 10;
		int x_5 = 10;
		int x_4 = 10 + 1 * 35;
		int x_3 = 10 + 2 * 35;
		int x_2 = 10 + 3 * 35;
		int x_1 = 10 + 4 * 35;
		int y_6 = 80;
		int y_5 = 80 + 1 * 40;
		int y_4 = 80 + 1 * 40;
		int y_3 = 80 + 1 * 40;
		int y_2 = 80 + 1 * 40;
		int y_1 = 80 + 1 * 40;
		XfsDrawBlock(display, w, x_6, y_6, 30, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 30, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 30, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 30, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 30, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 30, 20, "1");
	}
	else if (XfsGetArrangementOption() == 45) {
		int x_6 = 10;
		int x_5 = 10 + 1 * 30;
		int x_4 = 10 + 2 * 30;
		int x_3 = 10 + 3 * 30;
		int x_2 = 10 + 4 * 30;
		int x_1 = 10 + 5 * 30;
		int y_6 = 100;
		int y_5 = 100;
		int y_4 = 100;
		int y_3 = 100;
		int y_2 = 100;
		int y_1 = 100;
		XfsDrawBlock(display, w, x_6, y_6, 25, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 25, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 25, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 25, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 25, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 25, 20, "1");
	}
	else {
		int x_6 = 80;
		int x_5 = 80;
		int x_4 = 80;
		int x_3 = 80;
		int x_2 = 80;
		int x_1 = 80;
		int y_6 = 20;
		int y_5 = 20 + 1 * 35;
		int y_4 = 20 + 2 * 35;
		int y_3 = 20 + 3 * 35;
		int y_2 = 20 + 4 * 35;
		int y_1 = 20 + 5 * 35;
		XfsDrawBlock(display, w, x_6, y_6, 40, 20, "6");
		XfsDrawBlock(display, w, x_5, y_5, 40, 20, "5");
		XfsDrawBlock(display, w, x_4, y_4, 40, 20, "4");
		XfsDrawBlock(display, w, x_3, y_3, 40, 20, "3");
		XfsDrawBlock(display, w, x_2, y_2, 40, 20, "2");
		XfsDrawBlock(display, w, x_1, y_1, 40, 20, "1");
	}

	XDrawText(display, w, gc, width - 170, height - 10, ti, 1);
	XUnloadFont(display, font->fid);
}

void XfsFMDisplayEvent(Display *display, XEvent *e, Window *winret, XClassHint *class_hint, void *data) {
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
				XfsDrawFMDisplay(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;
		case ButtonRelease:
			if (e->xbutton.button == 1) {
				XfsDrawFMDisplay(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;
		case Expose:
			if (e->xexpose.count == 0) {
				XfsDrawFMDisplay(display, e->xany.window, x_return, y_return, width_return, height_return, class_hint->res_name, 0);
			}
			break;

		default:
			*winret = 0;
	}
}