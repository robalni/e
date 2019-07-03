#include "ui.hpp"


int
editor_main(int, char**) {
    if (!init_gui()) {
        ERROR("Program could not start");
        return 1;
    }

    List<Buffer> buffers = List<Buffer>::new_empty();

    Buffer buf = Buffer::new_empty(MemoryManager::new_default());
    List<Buffer>::Node list_node {buf, null, null};
    buffers.add_last(&list_node);

#define MAX_VIEWS 64
    View views[MAX_VIEWS];
    size_t n_views = 0;

    views[0] = View::new_into_buffer(&buf);

    View& active_view = views[0];

    Buffer::TmpCursor& cur = active_view.cursor;

    // Main loop
    for (;;) {
        Event ev = read_input();
        if (ev.type & Event::CHAR) {
            buf.insert_char_at_cursor(ev.keysym, cur);
            render_everything(active_view);  // TODO: Don't need to.
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
            render_everything(active_view);  // TODO: Don't need to.
        }
        if (ev.type & Event::QUIT) {
            break;
        }
        if (ev.type & Event::RENDER) {
            render_everything(active_view);
        }
    }

    return 0;
}
