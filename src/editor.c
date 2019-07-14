#include "common.c"
#include "os.h"
#include "ui.c"

#ifdef LINUX
#  include "os/linux.c"
#endif
#ifdef WINDOWS
#  include "os/windows.c"
#endif

#include "list.c"
#include "memory.c"
#include "buffer.c"
#include "view.c"

#ifdef LINUX
#  include "os/linux-ui.c"
#endif
#ifdef WINDOWS
#  include "os/windows-ui.c"
#endif


private void
buffer_modified_update_cursor(const Buffer* buf, TmpCursor* cur) {
    if (cur->pos.segment) {
        const struct LatestChange* lc = &buf->latest_change;
        bool cursor_too_old = buf->cursor_revision > cur->revision
            && lc->where <= cur->full_backup_index;
        if (cursor_too_old) {
            i32 to_add = 0;
            if (lc->where < cur->full_backup_index) {
                to_add = buf->latest_change.chars_added;
                if (lc->chars_added < 0 && cur->full_backup_index - lc->where
                                           > lc->chars_added) {
                    to_add = lc->where - cur->full_backup_index;
                }
            }
            cur->full_backup_index += to_add;
            *cur = buf_index_to_cursor(buf, cur->full_backup_index);
        }
    }
}

private void
buffer_modified(const Buffer* buf, ViewList* views) {
    View* view = views->active_view;
    buffer_modified_update_cursor(buf, &view->cursor);
    if (view->start_cursor.pos.segment == null) {
        view->start_cursor = buf_index_to_cursor(buf, 0);
    }
    buffer_modified_update_cursor(buf, &view->start_cursor);
}

private void
window_resized(ViewList* views, int width, int height) {
    View* view = views->active_view;
    view->width = width / fontw;
    view->height = height / fonth;
}

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

    Buffer buf;
    if (argc == 2) {
        buf = new_buffer_from_file(new_mem_default(), argv[1]);
    } else {
        buf = new_buffer_empty(new_mem_default());
    }
    View view = new_view_into_buffer(&buf);

    set_active_view(&views, &view);

    // Main loop
    for (;;) {
        Event ev = read_input();
        if (ev.type & EVENT_CHAR) {
            buf_insert_char_at_cursor(&buf, ev.keysym, &view.cursor);
            buffer_modified(&buf, &views);
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
            case KEY_UP:
                cur_up_line(&view.cursor);
                break;
            case KEY_DOWN:
                cur_down_line(&view.cursor);
                break;
            case KEY_HOME:
                cur_start_line(&view.cursor);
                break;
            case KEY_END:
                cur_end_line(&view.cursor);
                break;
            case KEY_BACKSPACE: {
                TmpCursor where_to_remove = view.cursor;
                cur_prev_char(&where_to_remove);
                if (!cursor_eq(&view.cursor, &where_to_remove)) {
                    buf_remove_range(&buf, where_to_remove, where_to_remove);
                    buffer_modified(&buf, &views);
                }
            } break;
            case KEY_DEL:
                if (cur_has_char(&view.cursor)) {
                    buf_remove_range(&buf, view.cursor, view.cursor);
                    buffer_modified(&buf, &views);
                }
                break;
            case KEY_PAGEUP: {
                u32 lines_to_scroll = view.height - 2;
                for (u32 i = 0; i < lines_to_scroll; i++) {
                    if (view.offset_y > 0) {
                        cur_up_line(&view.start_cursor);
                        view.offset_y--;
                    }
                }
            } break;
            case KEY_PAGEDOWN: {
                u32 lines_to_scroll = view.height - 2;
                for (u32 i = 0; i < lines_to_scroll; i++) {
                    cur_down_line(&view.start_cursor);
                    view.offset_y++;
                }
            } break;
            }
            render_everything(&view);  // TODO: Don't need to.
        }
        if (ev.type & EVENT_QUIT) {
            break;
        }
        if (ev.type & EVENT_RENDER) {
            render_everything(&view);
        }
        if (ev.type & EVENT_RESIZE) {
            window_resized(&views, ev.width, ev.height);
        }
    }

    return 0;
}
