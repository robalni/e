#pragma once

#include "common.hpp"


Result
init_gui();

struct Event {
    enum Type {
        UNKNOWN,
        QUIT,
        KEYDOWN,
    };
    Type type;
};

Event
read_input();
