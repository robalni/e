#pragma once

#include "common.hpp"
#include "view.hpp"


enum Key {
    UNKNOWN,
    UP,
    RIGHT,
    LEFT,
    DOWN,
    HOME,
    END,
    RETURN,
    BACKSPACE,
    DEL,
    ESCAPE,
    PAGEDOWN,
    PAGEUP,
};

Result
init_gui();

struct Event {
    enum Type {
        UNKNOWN = 0,
        CHAR = 1,
        KEYDOWN = 2,
        QUIT = 4,
        RENDER = 8,
    };
    int type;
    int keysym;
};

Event
read_input();

void
render_everything(const View& bv);

static inline bool
key_is_printable(int keysym) {
    return (keysym >= 32 && keysym < 127) || keysym == '\n';
}
