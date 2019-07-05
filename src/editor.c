#include "common.c"
#include "list.c"
#include "memory.c"
#include "buffer.c"
#include "view.c"
#include "ui.c"

#ifdef LINUX
#  include "os/linux.c"
#  include "os/linux-ui.c"
#endif
#ifdef WINDOWS
#  include "os/windows.c"
#  include "os/windows-ui.c"
#endif


// This function is directly called by the real main function found in
// the os directory.
public int
editor_main(int argc, char** argv) {
    if (!init_gui()) {
        ERROR("Program could not start");
        return 1;
    }

    BufferList buffers = {0};
    ViewList views = {0};

    Buffer buf = new_buffer_empty(new_mem_default());
    View view = new_view_into_buffer(&buf);

    set_active_view(&views, &view);

    // Main loop
    for (;;) {
        Event ev = read_input();
        if (ev.type & EVENT_CHAR) {
            buf_insert_char_at_cursor(&buf, ev.keysym, &view.cursor);
            render_everything(&view);  // TODO: Don't need to.
        }
        if (ev.type & EVENT_KEYDOWN) {
            switch (ev.keysym) {
            case KEY_RIGHT:
                cur_next_char(&view.cursor);
                break;
            case KEY_LEFT:
                cur_prev_char(&view.cursor);
                break;
            }
            render_everything(&view);  // TODO: Don't need to.
        }
        if (ev.type & EVENT_QUIT) {
            break;
        }
        if (ev.type & EVENT_RENDER) {
            render_everything(&view);
        }
    }

    return 0;
}
