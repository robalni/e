#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>


static Display* disp;
static Window win;
static int win_w, win_h;
static Pixmap win_buf;
static GC gc;
static Atom delete_window_message;
static XftFont *font;
static XftDraw *draw;
static Visual *vis;
static Colormap colormap;

static u64 bg = 0x222222;
static u64 fg = 0xffffff;
static u64 soft = 0x999999;
static u8 fontw = 10;
static u8 fonth = 20;

Result
init_gui() {
    disp = XOpenDisplay(null);
    if (!disp) {
        ERROR("Could not open display");
        return false;
    }

    Window root = DefaultRootWindow(disp);
    vis = DefaultVisual(disp, 0);
    win = XCreateWindow(disp, root, 0, 0, 80*10, 25*20, 0,
                        24, InputOutput, vis, 0, NULL);

    int events = ExposureMask | StructureNotifyMask | KeyPressMask;
    XSelectInput(disp, win, events);

    // Make it possible to receive window close events.
    delete_window_message = XInternAtom(disp, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(disp, win, &delete_window_message, 1);

    XMapWindow(disp, win);

    gc = XCreateGC(disp, win, 0, null);
    win_buf = XCreatePixmap(disp, win, 1, 1, 24);
    colormap = DefaultColormap(disp, 0);
    draw = XftDrawCreate(disp, win_buf, vis, colormap);
    font = XftFontOpenName(disp, 0, "monospace");

    return true;
}

static XRenderColor
xcolor(u32 color) {
    return (XRenderColor) {
        (u16)((color & 0xff0000) >> 8),
        (u16)((color & 0x00ff00)),
        (u16)((color & 0x0000ff) << 8),
        0xffff,
    };
}

static void
draw_char(char c, GC gc, int col, int row,
                      XRenderColor color) {
    XftColor xft_color = {0, color};
    int x = col * fontw + 1;
    int y = row * fonth + fonth * 0.75;
    XftDrawString8(draw, &xft_color, font, x, y, (FcChar8 *)&c, 1);
}

static void
draw_rect(GC gc, int col, int row, int w, int h, int color) {
    XSetForeground(disp, gc, color);
    XFillRectangle(disp, win_buf, gc, col * fontw, row * fonth, w * fontw,
                   h * fonth);
    XSetForeground(disp, gc, fg);
}

static void
draw_vline(GC gc, int col, int row, int count, int color) {
    XSetForeground(disp, gc, color);
    XFillRectangle(disp, win_buf, gc, col * fontw - 1, row * fonth + 1, 2,
                   count * fonth - 2);
    XSetForeground(disp, gc, fg);
}

void
render_everything(const View* bv) {
    // The background.
    draw_rect(gc, 0, 0, win_w / fontw + 1, win_h / fonth + 1, 0xff111111);

    TmpCursor cur = buf_cursor_at_start(bv->buffer);
    int start_col = 0;
    int row = 0;
    int linenr_width = 5;
    int col = start_col + linenr_width;
    int line = 1;
    bool line_start = true;
    XRenderColor xcolor_soft = xcolor(soft);
    XRenderColor xcolor_fg = xcolor(fg);
    for (; cur_has_char(&cur); cur_next_char(&cur)) {
        char c = cur_get_char(&cur);
        if (line_start) {
            char nr[11] = {0};
            snprintf(nr, 10, "% 4d ", line);
            col -= linenr_width;
            for (size_t j = 0; j < 5; j++) {
                draw_char(nr[j], gc, col, row, xcolor_soft);
                col++;
                line_start = false;
            }
            XSetForeground(disp, gc, fg);
        }
        if (cursor_eq(&bv->cursor, &cur)) {
            draw_vline(gc, col, row, 1, soft);
        }
        if (c == '\n') {
            row++;
            line++;
            col = start_col + linenr_width;
            draw_rect(gc, start_col, row, 80, 1, bg);
            line_start = true;
            continue;
        }
        draw_char(c, gc, col, row, xcolor_fg);
        col++;
        line_start = false;
    }
    if (cursor_eq(&cur, &bv->cursor)) {
        draw_vline(gc, col, row, 1, soft);
    }

    XCopyArea(disp, win_buf, win, gc, 0, 0, win_w, win_h, 0, 0);
}

static int
to_key_x(int k) {
    switch (k) {
    case XK_Up:
        return KEY_UP;
    case XK_Left:
        return KEY_LEFT;
    case XK_Right:
        return KEY_RIGHT;
    case XK_Down:
        return KEY_DOWN;
    case XK_Home:
        return KEY_HOME;
    case XK_End:
        return KEY_END;
    case XK_Return:
        return KEY_RETURN;
    case XK_BackSpace:
        return KEY_BACKSPACE;
    case XK_Delete:
        return KEY_DEL;
    case XK_Escape:
        return KEY_ESCAPE;
    case XK_Page_Down:
        return KEY_PAGEDOWN;
    case XK_Page_Up:
        return KEY_PAGEUP;
    default:
        return k;
    }
}

Event
read_input() {
    XEvent event;
    for (;;) {
        XNextEvent(disp, &event);
        switch (event.type) {
        case KeyPress: {
            KeySym sym = XLookupKeysym(&event.xkey, event.xkey.state & 7);
            Event e;
            e.type = EVENT_KEYDOWN;
            if (sym == XK_Return) {
                sym = '\n';
            }
            if (key_is_printable(sym)) {
                e.type |= EVENT_CHAR;
            }
            e.keysym = to_key_x(sym);
            return e;
        } break;
        case ConfigureNotify: {
            XConfigureEvent ce = event.xconfigure;
            if (ce.width != win_w || ce.height != win_h) {
                //printf("RESIZE %dx%d\n", ce.width, ce.height);
                XftDrawDestroy(draw);
                XFreePixmap(disp, win_buf);
                win_w = ce.width;
                win_h = ce.height;
                win_buf = XCreatePixmap(disp, win, win_w, win_h, 24);
                draw = XftDrawCreate(disp, win_buf, vis, colormap);
                Event e;
                e.type = EVENT_RENDER;
                return e;
            }
        } break;
        case Expose: {
            XExposeEvent xe = event.xexpose;
            //printf("EXPOSE %dx%d %dx%d\n", xe.x, xe.y, xe.width, xe.height);
            int x = xe.x, y = xe.y, w = xe.width, h = xe.height;
            XCopyArea(disp, win_buf, win, gc, x, y, w, h, x, y);
        } break;
        case ClientMessage: {
            Atom received_atom = (Atom)event.xclient.data.l[0];
            if (received_atom == delete_window_message) {
                Event e;
                e.type = EVENT_QUIT;
                return e;
            }
        } break;
        }
    }
    return (Event) {EVENT_UNKNOWN};
}
