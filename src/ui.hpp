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
        UNKNOWN,
        QUIT,
        KEYDOWN,
        RENDER,
    };
    Type type;
    int keysym;
};

Event
read_input();

void
render_everything(const View& bv);
