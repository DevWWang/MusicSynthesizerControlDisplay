#include <X11/Xlib.h>
#include <stdio.h>

int main (int argc, char **argv) {
    Window root_window; //<--one
    int root_x, root_y; //<--two
    unsigned int mask; //<--three

    Display *display = XOpenDisplay(NULL);

    /*
    Bool XQueryPointer(display, w, root_return, child_return, root_x_return, root_y_return,
                         win_x_return, win_y_return, mask_return)
          Display *display;
          Window w;
          Window *root_return, *child_return;
          int *root_x_return, *root_y_return;
          int *win_x_return, *win_y_return;
          unsigned int *mask_return;
    */

    XQueryPointer(display, DefaultRootWindow(display), &root_window, &root_window, &root_x, &root_y, &root_x, &root_y, &mask); //<--four

    printf("Mouse coordinates (X: %d, Y: %d)\n", root_x, root_y);

    XCloseDisplay(display);
    return 0;
}