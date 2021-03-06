#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>


static Display* disp;
static Window win;
static int win_w, win_h;
static Pixmap win_buf;
static GC gc;
static Atom delete_window_message;
static XftFont* font;
static XftDraw* draw;
static Visual* vis;
static Colormap colormap;

static u64 bg = 0x222222;
static u64 fg = 0xffffff;
static u64 soft = 0x999999;
static u8 fontw = 10;
static u8 fonth = 20;

public Result
gui_init() {
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

private XRenderColor
xcolor(u32 color) {
    return (XRenderColor) {
        (u16)((color & 0xff0000) >> 8),
        (u16)((color & 0x00ff00)),
        (u16)((color & 0x0000ff) << 8),
        0xffff,
    };
}

private void
draw_char(char c, GC gc, int col, int row,
                      XRenderColor color) {
    XftColor xft_color = {0, color};
    int x = col * fontw + 1;
    int y = row * fonth + fonth * 0.75;
    XftDrawString8(draw, &xft_color, font, x, y, (FcChar8*)&c, 1);
}

private void
draw_rect(GC gc, int col, int row, int w, int h, int color) {
    XSetForeground(disp, gc, color);
    XFillRectangle(disp, win_buf, gc, col * fontw, row * fonth, w * fontw,
                   h * fonth);
    XSetForeground(disp, gc, fg);
}

private void
draw_vline(GC gc, int col, int row, int count, int color) {
    XSetForeground(disp, gc, color);
    XFillRectangle(disp, win_buf, gc, col * fontw - 1, row * fonth + 1, 2,
                   count * fonth - 2);
    XSetForeground(disp, gc, fg);
}

public void
render_save_popup(GC gc) {
    XftColor xft_color = {0, xcolor(fg)};
    XftColor xft_bg_color = {0, xcolor(bg)};
    const char* label = "Hello";
    int x = 80, y = 80;
    draw_rect(gc, 7, 3, 20, 3, bg);
    XftDrawString8(draw, &xft_color, font, x, y, (FcChar8*)label, 5);
}

public void
render_buffer_view(const View* bv, u32 start_col, u32 start_row,
                   u32 width, u32 height) {
    const u32 linenr_width = 4;         // 4 digits
    const u32 left_margin = linenr_width + 1;
    u32 row = start_row;                // Absolute row in window.
    u32 col = start_col + left_margin;  // Absolute column in window.
    u32 vis_lines = 0;                  // Number of visual lines printed.
    u32 linenr = bv->offset_y + 1;
    u32 line_started_at_row = row;

    XRenderColor xcolor_soft = xcolor(soft);
    XRenderColor xcolor_fg = xcolor(fg);
    draw_rect(gc, start_col, row, width, 1, bg);

    TmpCursor cur = view_cursor_at_start(bv);
    for (; cur_has_char(&cur); cur_next_char(&cur)) {
        char c = bufpos_get_char(&cur.pos);
        bool hard_break = c == '\n';
        bool line_break = hard_break || col - start_col >= width;

        if (cursor_eq(&bv->cursor, &cur)) {
            draw_vline(gc, col, row, 1, soft);
        }
        if (hard_break) {
            char nr[11];
            snprintf(nr, 11, "% *d ", linenr_width, linenr);
            u32 c = start_col;
            for (size_t j = 0; j < 5; j++) {
                draw_char(nr[j], gc, c, line_started_at_row, xcolor_soft);
                c++;
            }
        }
        if (line_break) {
            vis_lines++;
            if (vis_lines >= height) {
                break;
            }
            row++;
            col = start_col + left_margin;

            draw_rect(gc, start_col, row, width, 1, bg);
        }
        if (c != '\n') {
            draw_char(c, gc, col, row, xcolor_fg);
            col++;
        }
        if (hard_break) {
            linenr++;
            line_started_at_row = row;
        }
    }
    if (cursor_eq(&cur, &bv->cursor)) {
        draw_vline(gc, col, row, 1, soft);
    }
}

public void
render_edit_frame(const ViewList* vl) {
    for (const ListNode(View)* v = vl->views.first; v; v = v->next) {
        render_buffer_view(&v->obj, 0, 0, 50, 20);
    }
}

public void
render_save_frame() {
}

public void
gui_render_everything(const FrameList* wl) {
    // The background.
    draw_rect(gc, 0, 0, win_w / fontw + 1, win_h / fonth + 1, 0xff111111);

    render_frame(&wl->frames.first->obj);

    XCopyArea(disp, win_buf, win, gc, 0, 0, win_w, win_h, 0, 0);
}

private int
to_key_x(int k) {
    switch (k) {
    case XK_Up:
        return EKEY_UP;
    case XK_Left:
        return EKEY_LEFT;
    case XK_Right:
        return EKEY_RIGHT;
    case XK_Down:
        return EKEY_DOWN;
    case XK_Home:
        return EKEY_HOME;
    case XK_End:
        return EKEY_END;
    case XK_BackSpace:
        return EKEY_BACKSPACE;
    case XK_Delete:
        return EKEY_DEL;
    case XK_Escape:
        return EKEY_ESCAPE;
    case XK_Page_Down:
        return EKEY_PAGEDOWN;
    case XK_Page_Up:
        return EKEY_PAGEUP;
    default:
        return k;
    }
}

public Event
gui_read_input() {
    XEvent event;
    for (;;) {
        XNextEvent(disp, &event);
        switch (event.type) {
        case KeyPress: {
            KeySym sym = XLookupKeysym(&event.xkey, event.xkey.state & 7);
            Event e;
            e.type = EVENT_KEYDOWN;
            if (key_is_printable(sym) || sym == XK_Return) {
                e.type |= EVENT_CHAR;
                if (sym == XK_Return) {
                    e.ch = '\n';
                } else {
                    e.ch = sym;
                }
            }
            sym = to_key_x(sym);
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
                e.type = EVENT_RENDER | EVENT_RESIZE;
                e.width = win_w;
                e.height = win_h;
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
