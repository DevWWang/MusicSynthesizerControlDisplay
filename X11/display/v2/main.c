#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include "Xfs.h"

#define NIL (0)

#define ltbrown3Color 0xe0e0e0
#define ltbrownColor 0xc0c0c0
#define brownColor 0x808080

int main(int argc, char *argv) {
	Window w;
	Window w_op_1, w_op_2, w_op_3, w_op_4, w_op_5, w_op_6;
	Window w_slider_1, w_slider_2;
	Window w_level_1;
	GC gc;
	int blackColor;
	int whiteColor;
	Display *dpy;
	XEvent e;
	int width, height;
	XFontStruct *font;
	char *greeting = "Hey!";
	XTextProperty ts;
	XClassHint class_hint;
	Window win_ret;
	int k, position, state;

	dpy = XfsInit(NIL);

	blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

	//w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 300, 300, 0, ltbrownColor, ltbrownColor);
	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 900, 600, 0, ltbrownColor, ltbrownColor);
	class_hint.res_name = "one";
	class_hint.res_class = "two";
	XSetClassHint(dpy, w, &class_hint);

	// Button Labels
	char *op_1 = "Operator 1";
	char *op_2 = "Operator 2";
	char *op_3 = "Operator 3";
	char *op_4 = "Operator 4";
	char *op_5 = "Operator 5";
	char *op_6 = "Operator 6";

	// First Button Position and its Dimensions
	int x_initial = 5;
	int y_initial = 10;

	int space = BUTTON_HEIGHT + 3;
	int y_op_2 = y_initial + space;
	int y_op_3 = y_op_2 + space;
	int y_op_4 = y_op_3 + space;
	int y_op_5 = y_op_4 + space;
	int y_op_6 = y_op_5 + space;
	// radio button
	w_op_1 = XfsCreateOperatorButton(dpy, w, x_initial, y_initial, BUTTON_WIDTH, BUTTON_HEIGHT, op_1);
	w_op_2 = XfsCreateOperatorButton(dpy, w, x_initial, y_op_2, BUTTON_WIDTH, BUTTON_HEIGHT, op_2);
	w_op_3 = XfsCreateOperatorButton(dpy, w, x_initial, y_op_3, BUTTON_WIDTH, BUTTON_HEIGHT, op_3);
	w_op_4 = XfsCreateOperatorButton(dpy, w, x_initial, y_op_4, BUTTON_WIDTH, BUTTON_HEIGHT, op_4);
	w_op_5 = XfsCreateOperatorButton(dpy, w, x_initial, y_op_5, BUTTON_WIDTH, BUTTON_HEIGHT, op_5);
	w_op_6 = XfsCreateOperatorButton(dpy, w, x_initial, y_op_6, BUTTON_WIDTH, BUTTON_HEIGHT, op_6);

	w_slider_1 = XfsCreateSlider(dpy, w, BUTTON_WIDTH + 15, y_initial, SLIDER_WIDTH, SLIDER_HEIGHT, "Latitude");
	w_slider_2 = XfsCreateSlider(dpy, w, BUTTON_WIDTH + 15, y_initial + 30, SLIDER_WIDTH, SLIDER_HEIGHT, "Time Stamp");

	XSelectInput(dpy, w, KeyPressMask | ButtonPressMask | StructureNotifyMask | ExposureMask);

	XMapWindow(dpy, w);

	gc = XCreateGC(dpy, w, 0 , NIL);

	font = XLoadQueryFont(dpy, "9x15");

	XSetForeground(dpy, gc, 0x00ff00);
	for(;;) {
		char buff[10];
		XComposeStatus compose;
		KeySym keysym;

		int state_op_1, state_op_2, state_op_3, state_op_4, state_op_5, state_op_6;
		int temp_slider_x, temp_slider_y;
		int state_level_1;
		XNextEvent(dpy, &e);
		XfsEventToolkitHandler(dpy, &e, &win_ret, (void *)&k);

		switch (e.type) {
			case KeyPress:
				XLookupString(&e.xkey, buff, 10, &keysym, &compose);
				fprintf(stderr, "%s\n", buff);
				if (!strcmp(buff, "q")) {
					XUnloadFont(dpy, font->fid);
					XFreeGC(dpy, gc);
					XCloseDisplay(dpy);
					exit(0);
				}
				break;

			case Expose:
				if (e.xexpose.count == 0) {
					XDrawString(dpy, w, gc,
								(width - XTextWidth(font, greeting, strlen(greeting))) / 2,
								(height - (font->ascent + font->descent)) / 2,
								greeting, strlen(greeting));
				}
				break;

			case ButtonPress:
				if (win_ret == w_op_1) {
					state_op_1 = XfsGetOperatorButtonState(w_op_1, 0);
					printf("Operator 1 is %s [%d]\n", state_op_1 ? "ON" : "OFF", state_op_1);
				}
				if (win_ret == w_op_2) {
					state_op_2 = XfsGetOperatorButtonState(w_op_2, 0);
					printf("Operator 2 is %s [%d]\n", state_op_2 ? "ON" : "OFF", state_op_2);
				}
				if (win_ret == w_op_3) {
					state_op_3 = XfsGetOperatorButtonState(w_op_3, 0);
					printf("Operator 3 is %s [%d]\n", state_op_3 ? "ON" : "OFF", state_op_3);
				}
				if (win_ret == w_op_4) {
					state_op_4 = XfsGetOperatorButtonState(w_op_4, 0);
					printf("Operator 4 is %s [%d]\n", state_op_4 ? "ON" : "OFF", state_op_4);
				}
				if (win_ret == w_op_5) {
					state_op_5 = XfsGetOperatorButtonState(w_op_5, 0);
					printf("Operator 5 is %s [%d]\n", state_op_5 ? "ON" : "OFF", state_op_5);
				}
				if (win_ret == w_op_6) {
					state_op_6 = XfsGetOperatorButtonState(w_op_6, 0);
					printf("Operator 6 is %s [%d]\n", state_op_6 ? "ON" : "OFF", state_op_6);
				}
				if (win_ret == w_slider_1) {
				}
				if (win_ret == w_slider_2) {
				}
				break;
		}
	}
	//sleep(10);
}