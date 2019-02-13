#include "ui.hpp"


int
editor_main(int, char**) {
    if (!init_gui()) {
        ERROR("Program could not start");
        return 1;
    }

    // Main loop
    for (;;) {
        Event ev = read_input();
        if (ev.type == Event::QUIT) {
            break;
        }
    }

    return 0;
}
