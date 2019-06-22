#pragma once

#include "common.hpp"
#include "view.hpp"


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
};

Event
read_input();

void
render_everything(const View& bv);
