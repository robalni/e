#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>


static Display* disp;
static Window win;
static int win_w, win_h;
static Pixmap win_buf;
static GC gc;
static Atom delete_window_message;

Result
init_gui() {
    disp = XOpenDisplay(null);
    if (!disp) {
        ERROR("Could not open display");
        return false;
    }

    Window root = DefaultRootWindow(disp);
    win = XCreateWindow(disp, root, 0, 0, 80*10, 25*20, 0,
                        24, InputOutput, CopyFromParent, 0, NULL);

    int events = ExposureMask | StructureNotifyMask;
    XSelectInput(disp, win, events);

    // Make it possible to receive window close events.
    delete_window_message = XInternAtom(disp, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(disp, win, &delete_window_message, 1);

    XMapWindow(disp, win);

    gc = XCreateGC(disp, win, 0, null);
    win_buf = XCreatePixmap(disp, win, 1, 1, 24);

    return true;
}

static void
render_everything() {
    XSetForeground(disp, gc, 0x222222);
    XFillRectangle(disp, win_buf, gc, 0, 0, win_w, win_h);
}

Event
read_input() {
    XEvent event;
    for (;;) {
        XNextEvent(disp, &event);
        switch (event.type) {
        case ConfigureNotify: {
            XConfigureEvent ce = event.xconfigure;
            if (ce.width != win_w || ce.height != win_h) {
                //printf("RESIZE %dx%d\n", ce.width, ce.height);
                XFreePixmap(disp, win_buf);
                win_w = ce.width;
                win_h = ce.height;
                win_buf = XCreatePixmap(disp, win, win_w, win_h, 24);
                render_everything();
            }
        } break;
        case Expose: {
            XExposeEvent xe = event.xexpose;
            //printf("EXPOSE %dx%d %dx%d\n", xe.x, xe.y, xe.width, xe.height);
            int x = xe.x, y = xe.y, w = xe.width, h = xe.height;
            XCopyArea(disp, win_buf, win, gc, x, y, w, h, x, y);
        } break;
        case ClientMessage: {
            Atom received_atom = static_cast<Atom>(event.xclient.data.l[0]);
            if (received_atom == delete_window_message) {
                return {Event::QUIT};
            }
        } break;
        }
    }
    // Should never happen.
    return {Event::UNKNOWN};
}
