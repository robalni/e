#include "ui.hpp"


int
editor_main(int, char**) {
    if (!init_gui()) {
        ERROR("Program could not start");
        return 1;
    }

    Buffer buf = Buffer::new_empty(MemoryManager::new_default());
    buf.insert_char('H', 0);
    buf.insert_char('i', 1);
    buf.insert_char('!', 2);
    buf.insert_char('\n', 3);
    View bv = View::new_into_buffer(&buf);

    Buffer::TmpCursor& cur = bv.cursor;

    // Main loop
    for (;;) {
        Event ev = read_input();
        if (ev.type & Event::CHAR) {
            buf.insert_char_at_cursor(ev.keysym, cur);
            render_everything(bv);  // TODO: Don't need to.
        }
        if (ev.type & Event::KEYDOWN) {
            switch (ev.keysym) {
            case Key::RIGHT:
                cur.next_char();
                break;
            case Key::LEFT:
                cur.prev_char();
                break;
            }
            render_everything(bv);  // TODO: Don't need to.
        }
        if (ev.type & Event::QUIT) {
            break;
        }
        if (ev.type & Event::RENDER) {
            render_everything(bv);
        }
    }

    return 0;
}
