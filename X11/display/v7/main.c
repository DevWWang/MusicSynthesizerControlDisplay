#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Xfs.h"
#include "terasic_os_includes.h"

#define NIL (0)

#define ltbrown3Color 0xe0e0e0
#define ltbrownColor 0xc0c0c0
#define brownColor 0x808080

#define REG_BASE 0xFF200000
#define REG_SPAN 0x00200000

int state_op_1, state_op_2, state_op_3, state_op_4, state_op_5, state_op_6;
int op_selected;
int op_coef_value, op_arr_value;
int modulation_coef, arrangment_no;
int state_sine, state_square, state_sawtooth, state_triangular;
int op_wave_selected;
int state_standard, state_loop;
int op_mode_selected;
int state_level_1, state_level_2, state_level_3, state_level_4, state_level_5, state_level_6, state_level_7;
int amp_1, amp_2, amp_3, amp_4, amp_5, amp_6, amp_7;
int time_1, time_2, time_3, time_4, time_5, time_6, time_7;
int saving_preset;
int current_level_set, current_amp_value, current_time_value;
int state_preset_1, state_preset_2, state_preset_3, state_preset_4, state_preset_5, state_preset_6, state_preset_7;
int preset_selected;
int loading[6][18], preset_loading[6][18];

int fd;
void* virtual_base;
extern void* input_arbitration_addr;

int main(int argc, char *argv) {

	fd = open("/dev/mem", ( O_RDWR | O_SYNC ) );
	virtual_base = mmap( NULL, REG_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, REG_BASE );
	input_arbitration_addr = virtual_base + 0x0;

	/* Parent Windiow */
	Window w;
	/* Children Windows */
	Window w_op_1, w_op_2, w_op_3, w_op_4, w_op_5, w_op_6;
	Window w_save, w_update;
	Window w_coef, w_arr;
	Window w_fm, w_drawing;
	Window w_sine, w_square, w_sawtooth, w_triangular;
	Window w_standard, w_loop;
	Window w_slider_1, w_slider_2;
	Window w_level_1, w_level_2, w_level_3, w_level_4, w_level_5, w_level_6, w_level_7;
	Window w_preset_1, w_preset_2, w_preset_3, w_preset_4, w_preset_5, w_preset_6, w_preset_7;
	GC gc;
	int blackColor;
	int whiteColor;
	Display *dpy;
	XEvent e;
	int width, height;
	XFontStruct *font;
	char *greeting = "";
	XTextProperty ts;
	XClassHint class_hint;
	Window win_ret;
	int k, position, state;

	/* Drawing Pen Setting */
	GC pen;
	XGCValues values;

	dpy = XfsInit(NIL);

	blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

	//w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 300, 300, 0, ltbrownColor, ltbrownColor);
	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 1000, 600, 0, ltbrownColor, ltbrownColor);
	class_hint.res_name = "one";
	class_hint.res_class = "two";
	XSetClassHint(dpy, w, &class_hint);

	// First Button Position and its Dimensions
	int x_op = 10;
	int y_op_1 = 20;
	int op_space = 35;
	int y_op_2 = y_op_1 + op_space;
	int y_op_3 = y_op_2 + op_space;
	int y_op_4 = y_op_3 + op_space;
	int y_op_5 = y_op_4 + op_space;
	int y_op_6 = y_op_5 + op_space;
	w_op_1 = XfsCreateOperatorButton(dpy, w, x_op, y_op_1, BUTTON_WIDTH, BUTTON_HEIGHT, "Operator 1");
	w_op_2 = XfsCreateOperatorButton(dpy, w, x_op, y_op_2, BUTTON_WIDTH, BUTTON_HEIGHT, "Operator 2");
	w_op_3 = XfsCreateOperatorButton(dpy, w, x_op, y_op_3, BUTTON_WIDTH, BUTTON_HEIGHT, "Operator 3");
	w_op_4 = XfsCreateOperatorButton(dpy, w, x_op, y_op_4, BUTTON_WIDTH, BUTTON_HEIGHT, "Operator 4");
	w_op_5 = XfsCreateOperatorButton(dpy, w, x_op, y_op_5, BUTTON_WIDTH, BUTTON_HEIGHT, "Operator 5");
	w_op_6 = XfsCreateOperatorButton(dpy, w, x_op, y_op_6, BUTTON_WIDTH, BUTTON_HEIGHT, "Operator 6");

	int x_op_slider_space = OPERATOR_SLIDER_WIDTH + 50;
	int x_op_coef = 150;
	int x_op_arr = x_op_coef + x_op_slider_space;
	w_coef = XfsCreateOperatorSlider(dpy, w, x_op_coef, y_op_1, OPERATOR_SLIDER_WIDTH, OPERATOR_SLIDER_HEIGHT, "Coef");
	w_arr = XfsCreateOperatorSlider(dpy, w, x_op_arr, y_op_1, OPERATOR_SLIDER_WIDTH, OPERATOR_SLIDER_HEIGHT, "Arr");

	int x_fm = 280;
	int y_fm = 30;
	w_fm = XfsCreateFMDisplay(dpy, w, x_fm, y_fm, XFS_DISPLAY_WIDTH, XFS_DISPLAY_HEIGHT, "FM");

	int x_drawing = 500;
	int y_drawing = 20;
	w_drawing = XfsCreateLevelDrawing(dpy, w, x_drawing, y_drawing, XFS_DRAWING_WIDTH, XFS_DRAWING_HEIGHT, "Coef");

	int x_wave = 10;
	int y_set_space = 50;
	int y_set_1 = y_op_6 + 100;
	int y_set_2 = y_set_1 + y_set_space;
	int y_set_3 = y_set_2 + y_set_space;
	int y_set_4 = y_set_3 + y_set_space;
	w_sine = XfsCreateOperatorButton(dpy, w, x_wave, y_set_1, BUTTON_WIDTH, BUTTON_HEIGHT, "Sine");
	w_square = XfsCreateOperatorButton(dpy, w, x_wave, y_set_2, BUTTON_WIDTH, BUTTON_HEIGHT, "Square");
	w_sawtooth = XfsCreateOperatorButton(dpy, w, x_wave, y_set_3, BUTTON_WIDTH, BUTTON_HEIGHT, "Sawtooth");
	w_triangular = XfsCreateOperatorButton(dpy, w, x_wave, y_set_4, BUTTON_WIDTH, BUTTON_HEIGHT, "Triangular");

	int x_mode = x_wave + 130;
	w_standard = XfsCreateOperatorButton(dpy, w, x_mode, y_set_2, BUTTON_WIDTH, BUTTON_HEIGHT, "One-Shot");
	w_loop = XfsCreateOperatorButton(dpy, w, x_mode, y_set_3, BUTTON_WIDTH, BUTTON_HEIGHT, "Loop");

	int x_level_set_1 = x_mode + 130;
	int x_level_set_space = 160;
	int x_level_set_2 = x_level_set_1 + x_level_set_space;
	int x_level_set_3 = x_level_set_2 + x_level_set_space;
	w_level_1 = XfsCreateLevelBox(dpy, w, x_level_set_1, y_set_2, LEVELBOX_WIDTH, LEVELBOX_HEIGHT, "a");
	w_level_2 = XfsCreateLevelBox(dpy, w, x_level_set_1, y_set_3, LEVELBOX_WIDTH, LEVELBOX_HEIGHT, "b");
	w_level_3 = XfsCreateLevelBox(dpy, w, x_level_set_1, y_set_4, LEVELBOX_WIDTH, LEVELBOX_HEIGHT, "c");
	w_level_4 = XfsCreateLevelBox(dpy, w, x_level_set_2, y_set_3, LEVELBOX_WIDTH, LEVELBOX_HEIGHT, "d");
	w_level_5 = XfsCreateLevelBox(dpy, w, x_level_set_2, y_set_4, LEVELBOX_WIDTH, LEVELBOX_HEIGHT, "e");
	w_level_6 = XfsCreateLevelBox(dpy, w, x_level_set_3, y_set_3, LEVELBOX_WIDTH, LEVELBOX_HEIGHT, "f");
	w_level_7 = XfsCreateLevelBox(dpy, w, x_level_set_3, y_set_4, LEVELBOX_WIDTH, LEVELBOX_HEIGHT, "g");

	int x_update = x_level_set_2 + 40;
	int y_update = y_set_2 + 5;
	w_update = XfsCreateOperatorButton(dpy, w, x_update, y_update, UPDATE_BUTTON_WIDTH, UPDATE_BUTTON_HEIGHT, "UPDATE");

	int x_save = x_update + UPDATE_BUTTON_WIDTH + 90;
	int y_save = y_set_2 + 5;
	w_save = XfsCreateOperatorButton(dpy, w, x_save, y_save, SAVE_BUTTON_WIDTH, SAVE_BUTTON_HEIGHT, "SAVE");

	int x_preset = 800;
	int y_preset_space = 35;
	int y_preset_1 = 330;
	int y_preset_2 = y_preset_1 + y_preset_space;
	int y_preset_3 = y_preset_2 + y_preset_space;
	int y_preset_4 = y_preset_3 + y_preset_space;
	int y_preset_5 = y_preset_4 + y_preset_space;
	int y_preset_6 = y_preset_5 + y_preset_space;
	int y_preset_7 = y_preset_6 + y_preset_space;
	w_preset_1 = XfsCreateOperatorButton(dpy, w, x_preset, y_preset_1, PRESET_BUTTON_WIDTH, PRESET_BUTTON_HEIGHT, "Preset-1");
	w_preset_2 = XfsCreateOperatorButton(dpy, w, x_preset, y_preset_2, PRESET_BUTTON_WIDTH, PRESET_BUTTON_HEIGHT, "Preset-2");
	w_preset_3 = XfsCreateOperatorButton(dpy, w, x_preset, y_preset_3, PRESET_BUTTON_WIDTH, PRESET_BUTTON_HEIGHT, "Preset-3");
	w_preset_4 = XfsCreateOperatorButton(dpy, w, x_preset, y_preset_4, PRESET_BUTTON_WIDTH, PRESET_BUTTON_HEIGHT, "Preset-4");
	w_preset_5 = XfsCreateOperatorButton(dpy, w, x_preset, y_preset_5, PRESET_BUTTON_WIDTH, PRESET_BUTTON_HEIGHT, "Preset-5");
	w_preset_6 = XfsCreateOperatorButton(dpy, w, x_preset, y_preset_6, PRESET_BUTTON_WIDTH, PRESET_BUTTON_HEIGHT, "Preset-6");
	w_preset_7 = XfsCreateOperatorButton(dpy, w, x_preset, y_preset_7, PRESET_BUTTON_WIDTH, PRESET_BUTTON_HEIGHT, "Preset-7");

	int x_slider = 15;
	int y_slider_space = 30;
	int y_slider_1 = 505;
	int y_slider_2 = y_slider_1 + y_slider_space;
	w_slider_1 = XfsCreateSlider(dpy, w, x_slider, y_slider_1, SLIDER_WIDTH, SLIDER_HEIGHT, "Amplitude");
	w_slider_2 = XfsCreateSlider(dpy, w, x_slider, y_slider_2, SLIDER_WIDTH, SLIDER_HEIGHT, "Time Stamp");

	/* Create the pen to draw lines with */
	values.foreground = blackColor;
	values.line_width = 1;
	values.line_style = LineSolid;
	pen = XCreateGC(dpy, w, GCForeground | GCLineWidth | GCLineStyle, &values);

	XSelectInput(dpy, w, KeyPressMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | ExposureMask);

	XMapWindow(dpy, w);

	gc = XCreateGC(dpy, w, 0 , NIL);

	font = XLoadQueryFont(dpy, "9x15");

	XSetForeground(dpy, gc, 0x00ff00);
	for(;;) {
		char buff[10];
		XComposeStatus compose;
		KeySym keysym;

		int temp_slider_1_x, temp_slider_2_x;

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
				XDrawString(dpy, w, pen, x_op_coef - 28, OPERATOR_SLIDER_HEIGHT + 35, "Modulation", strlen("Modulation"));
				XDrawString(dpy, w, pen, x_op_coef - 28, OPERATOR_SLIDER_HEIGHT + 45, "Coefficient", strlen("Coefficient"));
				XDrawString(dpy, w, pen, x_op_arr - 10, OPERATOR_SLIDER_HEIGHT + 35, "Algorithm", strlen("Algorithm"));
				XDrawString(dpy, w, pen, x_op_arr - 10, OPERATOR_SLIDER_HEIGHT + 45, "Options", strlen("Options"));
				XDrawString(dpy, w, pen, x_slider + SLIDER_WIDTH + 10, y_slider_1 + 13, "Amplitude", strlen("Amplitude"));
				XDrawString(dpy, w, pen, x_slider + SLIDER_WIDTH + 10, y_slider_2 + 13, "Time", strlen("Time"));
				XDrawString(dpy, w, pen, x_slider, y_slider_2 + 30, "2.46ms", strlen("2.46ms"));
				XDrawString(dpy, w, pen, x_slider + 190, y_slider_2 + 30, "10.07ms", strlen("10.07ms"));
				XDrawString(dpy, w, pen, x_slider + 280, y_slider_2 + 30, "100ms", strlen("100ms"));
				XDrawString(dpy, w, pen, x_slider + 440, y_slider_2 + 30, "503ms", strlen("503ms"));
				XDrawString(dpy, w, pen, x_slider + 540, y_slider_2 + 30, "1s", strlen("1s"));
				XDrawString(dpy, w, pen, x_slider + 590, y_slider_2 + 30, "2s", strlen("2s"));
				XDrawString(dpy, w, pen, x_slider + 630, y_slider_2 + 30, "20s", strlen("20s"));
				break;

			case ButtonPress:
				if (win_ret == w_op_1) {
					state_op_1 = XfsGetOperatorButtonState(w_op_1, 0);
					//printf("Operator 1 is %s [%d]\n", state_op_1 ? "ON" : "OFF", state_op_1);
					XfsSetOperatorSelected(state_op_1, state_op_2, state_op_3, state_op_4, state_op_5, state_op_6);
					op_selected = XfsGetOperatorNumber();
				}

				if (win_ret == w_op_2) {
					state_op_2 = XfsGetOperatorButtonState(w_op_2, 0);
					//printf("Operator 2 is %s [%d]\n", state_op_2 ? "ON" : "OFF", state_op_2);
					XfsSetOperatorSelected(state_op_1, state_op_2, state_op_3, state_op_4, state_op_5, state_op_6);
					op_selected = XfsGetOperatorNumber();
				}

				if (win_ret == w_op_3) {
					state_op_3 = XfsGetOperatorButtonState(w_op_3, 0);
					//printf("Operator 3 is %s [%d]\n", state_op_3 ? "ON" : "OFF", state_op_3);
					XfsSetOperatorSelected(state_op_1, state_op_2, state_op_3, state_op_4, state_op_5, state_op_6);
					op_selected = XfsGetOperatorNumber();
				}

				if (win_ret == w_op_4) {
					state_op_4 = XfsGetOperatorButtonState(w_op_4, 0);
					//printf("Operator 4 is %s [%d]\n", state_op_4 ? "ON" : "OFF", state_op_4);
					XfsSetOperatorSelected(state_op_1, state_op_2, state_op_3, state_op_4, state_op_5, state_op_6);
					op_selected = XfsGetOperatorNumber();
				}

				if (win_ret == w_op_5) {
					state_op_5 = XfsGetOperatorButtonState(w_op_5, 0);
					//printf("Operator 5 is %s [%d]\n", state_op_5 ? "ON" : "OFF", state_op_5);
					XfsSetOperatorSelected(state_op_1, state_op_2, state_op_3, state_op_4, state_op_5, state_op_6);
					op_selected = XfsGetOperatorNumber();
				}

				if (win_ret == w_op_6) {
					state_op_6 = XfsGetOperatorButtonState(w_op_6, 0);
					//printf("Operator 6 is %s [%d]\n", state_op_6 ? "ON" : "OFF", state_op_6);
					XfsSetOperatorSelected(state_op_1, state_op_2, state_op_3, state_op_4, state_op_5, state_op_6);
					op_selected = XfsGetOperatorNumber();
				}

				if (win_ret == w_sine) {
					state_sine = XfsGetOperatorButtonState(w_sine, 0);
					//printf("Sine is %s [%d]\n", state_sine ? "ON" : "OFF", state_sine);
					XfsSetWaveformSelected(state_sine, state_square, state_sawtooth, state_triangular);
					op_wave_selected = XfsGetWavefrom();
					if (saving_preset != 1) {
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 15, op_wave_selected);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 15, op_wave_selected);
					}
				}

				if (win_ret == w_square) {
					state_square = XfsGetOperatorButtonState(w_square, 0);
					//printf("Square is %s [%d]\n", state_square ? "ON" : "OFF", state_square);
					XfsSetWaveformSelected(state_sine, state_square, state_sawtooth, state_triangular);
					op_wave_selected = XfsGetWavefrom();
					if (saving_preset != 1) {
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 15, op_wave_selected);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 15, op_wave_selected);
					}
				}

				if (win_ret == w_sawtooth) {
					state_sawtooth = XfsGetOperatorButtonState(w_sawtooth, 0);
					//printf("Sawtooth is %s [%d]\n", state_sawtooth ? "ON" : "OFF", state_sawtooth);
					XfsSetWaveformSelected(state_sine, state_square, state_sawtooth, state_triangular);
					op_wave_selected = XfsGetWavefrom();
					if (saving_preset != 1) {
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 15, op_wave_selected);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 15, op_wave_selected);
					}
				}

				if (win_ret == w_triangular) {
					state_triangular = XfsGetOperatorButtonState(w_triangular, 0);
					//printf("Triangular is %s [%d]\n", state_triangular ? "ON" : "OFF", state_triangular);
					XfsSetWaveformSelected(state_sine, state_square, state_sawtooth, state_triangular);
					op_wave_selected = XfsGetWavefrom();
					if (saving_preset != 1) {
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 15, op_wave_selected);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 15, op_wave_selected);
					}
				}

				if (win_ret == w_standard) {
					state_standard = XfsGetOperatorButtonState(w_standard, 0);
					//printf("One-shot is %s [%d]\n", state_standard ? "ON" : "OFF", state_standard);
					XfsSetModeSelected(state_standard, state_loop);
					op_mode_selected = XfsGetMode();
					if (saving_preset != 1) {
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 0, op_mode_selected);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 0, op_mode_selected);
					}
				}

				if (win_ret == w_loop) {
					state_loop = XfsGetOperatorButtonState(w_loop, 0);
					//printf("Loop is %s [%d]\n", state_loop ? "ON" : "OFF", state_loop);
					XfsSetModeSelected(state_standard, state_loop);
					op_mode_selected = XfsGetMode();
					if (saving_preset != 1) {
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 0, op_mode_selected);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 0, op_mode_selected);
					}
				}

				if (win_ret == w_level_1) {
					//state_level_1 = XfsGetLevelBoxState(w_level_1, 0);
					XfsSetLevelBox("a", temp_slider_1_x, temp_slider_2_x);
					amp_1 = XfsGetAmplitude("a");
					time_1 = XfsGetTimeInterval("a");
					//current_level_set = XfsGetLevelSet("a");
					//current_amp_value = amp_1;
					//current_time_value = time_1;
					//DataTransmitted(TYPE_USER_SETTING, 0, op_selected, op_wave_selected, op_mode_selected, current_level_set, current_amp_value, current_time_value, op_coef_value, op_arr_value);
					if (saving_preset != 1) {
						printf("Sending Operator %d Level 1 ...\n", op_selected);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 1, amp_1);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 2, time_1);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						printf("Saving Operator %d Level 1 in Preset %d ...\n", op_selected, preset_selected);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 1, amp_1);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 2, time_1);
					}
				}

				if (win_ret == w_level_2) {
					//state_level_2 = XfsGetLevelBoxState(w_level_2, 0);
					XfsSetLevelBox("b", temp_slider_1_x, temp_slider_2_x);
					amp_2 = XfsGetAmplitude("b");
					time_2 = XfsGetTimeInterval("b");
					//current_level_set = XfsGetLevelSet("b");
					//current_amp_value = amp_2;
					//current_time_value = time_2;
					//DataTransmitted(TYPE_USER_SETTING, 0, op_selected, op_wave_selected, op_mode_selected, current_level_set, current_amp_value, current_time_value, op_coef_value, op_arr_value);
					if (saving_preset != 1) {
						printf("Sending Operator %d Level 2 ...\n", op_selected);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 3, amp_2);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 4, time_2);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						printf("Saving Operator %d Level 2 in Preset %d ...\n", op_selected, preset_selected);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 3, amp_2);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 4, time_2);
					}
				}

				if (win_ret == w_level_3) {
					//state_level_3 = XfsGetLevelBoxState(w_level_3, 0);
					XfsSetLevelBox("c", temp_slider_1_x, temp_slider_2_x);
					amp_3 = XfsGetAmplitude("c");
					time_3 = XfsGetTimeInterval("c");
					//current_level_set = XfsGetLevelSet("c");
					//current_amp_value = amp_3;
					//current_time_value = time_3;
					//DataTransmitted(TYPE_USER_SETTING, 0, op_selected, op_wave_selected, op_mode_selected, current_level_set, current_amp_value, current_time_value, op_coef_value, op_arr_value);
					if (saving_preset != 1) {
						printf("Sending Operator %d Level 3 ...\n", op_selected);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 5, amp_3);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 6, time_3);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						printf("Saving Operator %d Level 3 in Preset %d ...\n", op_selected, preset_selected);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 5, amp_3);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 6, time_3);
					}
				}

				if (win_ret == w_level_4) {
					//state_level_4 = XfsGetLevelBoxState(w_level_4, 0);
					XfsSetLevelBox("d", temp_slider_1_x, temp_slider_2_x);
					amp_4 = XfsGetAmplitude("d");
					time_4 = XfsGetTimeInterval("d");
					//current_level_set = XfsGetLevelSet("d");
					//current_amp_value = amp_4;
					//current_time_value = time_4;
					//DataTransmitted(TYPE_USER_SETTING, 0, op_selected, op_wave_selected, op_mode_selected, current_level_set, current_amp_value, current_time_value, op_coef_value, op_arr_value);
					if (saving_preset != 1) {
						printf("Sending Operator %d Level 4 ...\n", op_selected);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 7, amp_4);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 8, time_4);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						printf("Saving Operator %d Level 4 in Preset %d ...\n", op_selected, preset_selected);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 7, amp_4);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 8, time_4);
					}
				}

				if (win_ret == w_level_5) {
					//state_level_5 = XfsGetLevelBoxState(w_level_5, 0);
					XfsSetLevelBox("e", temp_slider_1_x, temp_slider_2_x);
					amp_5 = XfsGetAmplitude("e");
					time_5 = XfsGetTimeInterval("e");
					//current_level_set = XfsGetLevelSet("e");
					//current_amp_value = amp_5;
					//current_time_value = time_5;
					//DataTransmitted(TYPE_USER_SETTING, 0, op_selected, op_wave_selected, op_mode_selected, current_level_set, current_amp_value, current_time_value, op_coef_value, op_arr_value);
					if (saving_preset != 1) {
						printf("Sending Operator %d Level 5 ...\n", op_selected);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 9, amp_5);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 10, time_5);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						printf("Saving Operator %d Level 5 in Preset %d ...\n", op_selected, preset_selected);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 9, amp_5);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 10, time_5);
					}
				}

				if (win_ret == w_level_6) {
					//state_level_6 = XfsGetLevelBoxState(w_level_6, 0);
					XfsSetLevelBox("f", temp_slider_1_x, temp_slider_2_x);
					amp_6 = XfsGetAmplitude("f");
					time_6 = XfsGetTimeInterval("f");
					//current_level_set = XfsGetLevelSet("f");
					//current_amp_value = amp_6;
					//current_time_value = time_6;
					//DataTransmitted(TYPE_USER_SETTING, 0, op_selected, op_wave_selected, op_mode_selected, current_level_set, current_amp_value, current_time_value, op_coef_value, op_arr_value);
					if (saving_preset != 1) {
						printf("Sending Operator %d Level 6 ...\n", op_selected);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 11, amp_6);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 12, time_6);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						printf("Saving Operator %d Level 6 in Preset %d ...\n", op_selected, preset_selected);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 11, amp_6);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 12, time_6);
					}
				}

				if (win_ret == w_level_7) {
					//state_level_7 = XfsGetLevelBoxState(w_level_7, 0);
					XfsSetLevelBox("g", temp_slider_1_x, temp_slider_2_x);
					amp_7 = XfsGetAmplitude("g");
					time_7 = XfsGetTimeInterval("g");
					//current_level_set = XfsGetLevelSet("g");
					//current_amp_value = amp_7;
					//current_time_value = time_7;
					//DataTransmitted(TYPE_USER_SETTING, 0, op_selected, op_wave_selected, op_mode_selected, current_level_set, current_amp_value, current_time_value, op_coef_value, op_arr_value);
					if (saving_preset != 1) {
						printf("Sending Operator %d Level 7 ...\n", op_selected);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 13, amp_7);
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 14, time_7);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						printf("Saving Operator %d Level 7 in Preset %d ...\n", op_selected, preset_selected);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 13, amp_7);
						XfsSavingPreset(saving_preset, preset_selected, op_selected - 1, 14, time_7);
					}
				}

				if (win_ret == w_save) {
					//DataTransmitted(TYPE_USER_SETTING, 0, op_selected, op_wave_selected, op_mode_selected, current_level_set, current_amp_value, current_time_value, op_coef_value, op_arr_value);
					saving_preset = XfsGetOperatorButtonState(w_save, 0);
					printf("saving? %s\n", saving_preset? "YES" : "NO", saving_preset);
				}

				if (win_ret == w_update) {
					int i, j, count;
					printf("update\n");
					printf("Sending Coefficient & Algorithm ...\n");
					for (i = 0; i < 6; i++) {
						XfsSavingPreset(1, TEMP_STORAGE, op_selected - 1, 17, arrangment_no);
					}
					if (saving_preset != 1) {
						printPresetStorage(TEMP_STORAGE);
						XfsLoadPreset(saving_preset, TEMP_STORAGE, loading);
						for (i = 0; i < 6; i++) {
							count = 1;
							for (j = 1; j < 15; j = j + 2) {
								DataTransmitted(TYPE_USER_SETTING, 1, i + 1, loading[i][15], loading[i][0], count, loading[i][j], loading[i][j + 1], loading[i][16], loading[i][17]);
								count ++;
							}
						}
					}
				}

				if (win_ret == w_preset_1) {
					state_preset_1 = XfsGetOperatorButtonState(w_preset_1, 0);
					XfsSetPresetSelected(state_preset_1, state_preset_2, state_preset_3, state_preset_4, state_preset_5, state_preset_6, state_preset_7);
					preset_selected = XfsGetPresetNumber();
					//DataTransmitted(TYPE_PRESET, 1, 0, 0, 0, 0, 0, 0, 0, 0);
					//printf("preset-1\n");
					if (saving_preset != 1 && preset_selected == PRESET_1 && state_preset_1 == 1) {
						printPresetStorage(preset_selected);
						XfsLoadPreset(saving_preset, preset_selected, preset_loading);
						int i, j, count;
						for (i = 0; i < 6; i++) {
							count = 1;
							for (j = 1; j < 15; j = j + 2) {
								DataTransmitted(TYPE_USER_SETTING, 1, i + 1, preset_loading[i][15], preset_loading[i][0], count, preset_loading[i][j], preset_loading[i][j + 1], preset_loading[i][16], preset_loading[i][17]);
								count ++;
							}
						}
					}
				}

				if (win_ret == w_preset_2) {
					state_preset_2 = XfsGetOperatorButtonState(w_preset_2, 0);
					XfsSetPresetSelected(state_preset_1, state_preset_2, state_preset_3, state_preset_4, state_preset_5, state_preset_6, state_preset_7);
					preset_selected = XfsGetPresetNumber();
					//DataTransmitted(TYPE_PRESET, 2, 0, 0, 0, 0, 0, 0, 0, 0);
					//printf("preset-2\n");
					if (saving_preset != 1 && preset_selected == PRESET_2 && state_preset_2 == 1) {
						printPresetStorage(preset_selected);
						XfsLoadPreset(saving_preset, preset_selected, preset_loading);
						int i, j, count;
						for (i = 0; i < 6; i++) {
							count = 1;
							for (j = 1; j < 15; j = j + 2) {
								DataTransmitted(TYPE_USER_SETTING, 1, i + 1, preset_loading[i][15], preset_loading[i][0], count, preset_loading[i][j], preset_loading[i][j + 1], preset_loading[i][16], preset_loading[i][17]);
								count ++;
							}
						}
					}
				}

				if (win_ret == w_preset_3) {
					state_preset_3 = XfsGetOperatorButtonState(w_preset_3, 0);
					XfsSetPresetSelected(state_preset_1, state_preset_2, state_preset_3, state_preset_4, state_preset_5, state_preset_6, state_preset_7);
					preset_selected = XfsGetPresetNumber();
					//DataTransmitted(TYPE_PRESET, 3, 0, 0, 0, 0, 0, 0, 0, 0);
					//printf("preset-3\n");
					if (saving_preset != 1 && preset_selected == PRESET_3 && state_preset_3 == 1) {
						printPresetStorage(preset_selected);
						XfsLoadPreset(saving_preset, preset_selected, preset_loading);
						int i, j, count;
						for (i = 0; i < 6; i++) {
							count = 1;
							for (j = 1; j < 15; j = j + 2) {
								DataTransmitted(TYPE_USER_SETTING, 1, i + 1, preset_loading[i][15], preset_loading[i][0], count, preset_loading[i][j], preset_loading[i][j + 1], preset_loading[i][16], preset_loading[i][17]);
								count ++;
							}
						}
					}
				}

				if (win_ret == w_preset_4) {
					state_preset_4 = XfsGetOperatorButtonState(w_preset_4, 0);
					XfsSetPresetSelected(state_preset_1, state_preset_2, state_preset_3, state_preset_4, state_preset_5, state_preset_6, state_preset_7);
					preset_selected = XfsGetPresetNumber();
					//DataTransmitted(TYPE_PRESET, 4, 0, 0, 0, 0, 0, 0, 0, 0);
					//printf("preset-4\n");
					if (saving_preset != 1 && preset_selected == PRESET_4 && state_preset_4 == 1) {
						printPresetStorage(preset_selected);
						XfsLoadPreset(saving_preset, preset_selected, preset_loading);
						int i, j, count;
						for (i = 0; i < 6; i++) {
							count = 1;
							for (j = 1; j < 15; j = j + 2) {
								DataTransmitted(TYPE_USER_SETTING, 1, i + 1, preset_loading[i][15], preset_loading[i][0], count, preset_loading[i][j], preset_loading[i][j + 1], preset_loading[i][16], preset_loading[i][17]);
								count ++;
							}
						}
					}
				}

				if (win_ret == w_preset_5) {
					state_preset_5 = XfsGetOperatorButtonState(w_preset_5, 0);
					XfsSetPresetSelected(state_preset_1, state_preset_2, state_preset_3, state_preset_4, state_preset_5, state_preset_6, state_preset_7);
					preset_selected = XfsGetPresetNumber();
					//DataTransmitted(TYPE_PRESET, 5, 0, 0, 0, 0, 0, 0, 0, 0);
					//printf("preset-5\n");
					if (saving_preset != 1 && preset_selected == PRESET_5 && state_preset_5 == 1) {
						printPresetStorage(preset_selected);
						XfsLoadPreset(saving_preset, preset_selected, preset_loading);
						int i, j, count;
						for (i = 0; i < 6; i++) {
							count = 1;
							for (j = 1; j < 15; j = j + 2) {
								DataTransmitted(TYPE_USER_SETTING, 1, i + 1, preset_loading[i][15], preset_loading[i][0], count, preset_loading[i][j], preset_loading[i][j + 1], preset_loading[i][16], preset_loading[i][17]);
								count ++;
							}
						}
					}
				}

				if (win_ret == w_preset_6) {
					state_preset_6 = XfsGetOperatorButtonState(w_preset_6, 0);
					XfsSetPresetSelected(state_preset_1, state_preset_2, state_preset_3, state_preset_4, state_preset_5, state_preset_6, state_preset_7);
					preset_selected = XfsGetPresetNumber();
					//DataTransmitted(TYPE_PRESET, 6, 0, 0, 0, 0, 0, 0, 0, 0);
					//printf("preset-6\n");
					if (saving_preset != 1 && preset_selected == PRESET_6 && state_preset_6 == 1) {
						printPresetStorage(preset_selected);
						XfsLoadPreset(saving_preset, preset_selected, preset_loading);
						int i, j, count;
						for (i = 0; i < 6; i++) {
							count = 1;
							for (j = 1; j < 15; j = j + 2) {
								DataTransmitted(TYPE_USER_SETTING, 1, i + 1, preset_loading[i][15], preset_loading[i][0], count, preset_loading[i][j], preset_loading[i][j + 1], preset_loading[i][16], preset_loading[i][17]);
								count ++;
							}
						}
					}
				}

				if (win_ret == w_preset_7) {
					state_preset_7 = XfsGetOperatorButtonState(w_preset_7, 0);
					XfsSetPresetSelected(state_preset_1, state_preset_2, state_preset_3, state_preset_4, state_preset_5, state_preset_6, state_preset_7);
					preset_selected = XfsGetPresetNumber();
					//DataTransmitted(TYPE_PRESET, 7, 0, 0, 0, 0, 0, 0, 0, 0);
					//printf("preset-7\n");
					if (saving_preset != 1 && preset_selected == PRESET_7 && state_preset_7 == 1) {
						printPresetStorage(preset_selected);
						XfsLoadPreset(saving_preset, preset_selected, preset_loading);
						int i, j, count;
						for (i = 0; i < 6; i++) {
							count = 1;
							for (j = 1; j < 15; j = j + 2) {
								DataTransmitted(TYPE_USER_SETTING, 1, i + 1, preset_loading[i][15], preset_loading[i][0], count, preset_loading[i][j], preset_loading[i][j + 1], preset_loading[i][16], preset_loading[i][17]);
								count ++;
							}
						}
					}
				}

				if (win_ret == w_fm) {
					modulation_coef = XfsGetModulationCoefficient();
					arrangment_no = XfsGetArrangementOption();
					if (saving_preset != 1) {
						XfsSavingPresetForOperator(1, TEMP_STORAGE, op_selected - 1, 16, modulation_coef);
					}
					if (saving_preset == 1 && preset_selected != 0) {
						printf("Saving Coefficient & Algorithm in Preset %d ...\n", preset_selected);
						int i;
						for (i = 0; i < 6; i++) {
							XfsSavingPresetForOperator(saving_preset, preset_selected, i, 17, arrangment_no);
						}
						XfsSavingPresetForOperator(saving_preset, preset_selected, op_selected - 1, 16, modulation_coef);
					}
				}

				if (win_ret == w_drawing) {
				}
				break;

			default:
				if (win_ret == w_coef) {
					op_coef_value = XfsCalculateOperatorSlider(TYPE_COEF, XfsGetOperatorSliderControlPosY());
					//printf("COEF = %d\n", op_coef_value);
				}

				if (win_ret == w_arr) {
					op_arr_value = XfsCalculateOperatorSlider(TYPE_ARR, XfsGetOperatorSliderControlPosY());
					//printf("ARR = %d\n", op_arr_value);
				}

				if (win_ret == w_slider_1) {
					temp_slider_1_x = XfsCalculateSlider(TYPE_A, XfsGetSliderControlPosX());
					//printf("Amp = %d\n", temp_slider_1_x);
				}

				if (win_ret == w_slider_2) {
					temp_slider_2_x = XfsCalculateSlider(TYPE_T, XfsGetSliderControlPosX());
					//printf("Time = %d\n", temp_slider_2_x);
				}

				break;
		}
	}
	//sleep(10);
}