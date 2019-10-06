// This is UI code that is common for all operating systems.  More
// specific UI code for different operating systems can be found in
// the os directory.

enum Key {
    EKEY_UNKNOWN,
    EKEY_UP,
    EKEY_RIGHT,
    EKEY_LEFT,
    EKEY_DOWN,
    EKEY_HOME,
    EKEY_END,
    EKEY_BACKSPACE,
    EKEY_DEL,
    EKEY_ESCAPE,
    EKEY_PAGEDOWN,
    EKEY_PAGEUP,
};
typedef enum Key Key;

enum EventType {
    EVENT_UNKNOWN = 0,
    EVENT_CHAR    = 1,
    EVENT_KEYDOWN = 2,
    EVENT_QUIT    = 4,
    EVENT_RENDER  = 8,
    EVENT_RESIZE  = 16,
};
struct Event {
    int type;
    int keysym;  // EVENT_KEYDOWN
    char ch;     // EVENT_CHAR
    int width;   // EVENT_RESIZE
    int height;  // EVENT_RESIZE
};
typedef struct Event Event;

public bool
key_is_printable(int keysym) {
    return (keysym >= 32 && keysym < 127) || keysym == '\n';
}

struct View;

public void
render_buffer_view(const struct View* bv, u32 start_col, u32 start_row,
                   u32 width, u32 height);

struct Frame;

public void
render_frame(const struct Frame* w);


struct FrameList;

#ifdef HAVE_GUI

public Result
gui_init();

public void
gui_cleanup();

public void
gui_render_everything(const struct FrameList*);

public Event
gui_read_input();

#endif

#ifdef HAVE_TUI

public Result
tui_init();

public void
tui_cleanup();

public void
tui_render_everything(const struct FrameList*);

public Event
tui_read_input();

#endif

public Result
ui_init(bool gui) {
#ifdef HAVE_GUI
    if (gui) {
        return gui_init();
    }
#endif
#ifdef HAVE_TUI
    return tui_init();
#endif
}

public void
ui_cleanup(bool gui) {
#ifdef HAVE_GUI
    if (gui) {
        return gui_cleanup();
    }
#endif
#ifdef HAVE_TUI
    return tui_cleanup();
#endif
}

public void
ui_render_everything(bool gui, const struct FrameList* frames) {
#ifdef HAVE_GUI
    if (gui) {
        return gui_render_everything(frames);
    }
#endif
#ifdef HAVE_TUI
    return tui_render_everything(frames);
#endif
}

public Event
ui_read_input(bool gui) {
#ifdef HAVE_GUI
    if (gui) {
        return gui_read_input();
    }
#endif
#ifdef HAVE_TUI
    return tui_read_input();
#endif
}
