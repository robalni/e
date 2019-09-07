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
#include "ui-items.c"
#include "view.c"
#include "frame.c"

#ifdef LINUX
#  ifdef HAVE_X11
#    include "os/linux-ui.c"
#  endif
#  include "os/ncurses.c"
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
    View* view = get_active_view(views);
    buffer_modified_update_cursor(buf, &view->cursor);
    if (view->start_cursor.pos.segment == null) {
        view->start_cursor = buf_index_to_cursor(buf, 0);
    }
    buffer_modified_update_cursor(buf, &view->start_cursor);
}

private void
update_save_frame(FrameList* wl, const Event* ev) {
    Frame* w = get_active_frame(wl);
    struct SaveFrame* frame = &w->save;
    ViewList* views = &frame->edit_frame->vl;
    BufferList* buffers = &frame->edit_frame->bl;

    TextboxEvent te = update_textbox(ev);
    switch (te.type) {
    case TEXTBOX_SUBMIT: {
        char filename[50];
        const Buffer* buf = frame->buf;
        buf_get_content(buf, filename, sizeof filename);

        view_close_active(views, buffers);
        buf = get_active_view(views)->buffer;
        buf_write_file(buf, filename);
    } break;
    }

    // TODO: The rest of this function is old code, remove.
#if 0
    w = get_active_frame(wl);
    frame = &w->save;
    views = &frame->views;
    if (ev->type & EVENT_CHAR) {
        if (ev->ch == '\n') {
            char filename[50];
            const Buffer* buf = frame->buf;
            buf_get_content(buf, filename, sizeof filename);

            view_close_active(views, buffers);
            buf = get_active-view(views)->buffer;
            buf_write_file(buf, filename);
        }
    }
#endif
}

private void
update_edit_frame(FrameList* wl, Event* ev) {
    Frame* w = get_active_frame(wl);
    struct EditFrame* frame = &w->edit;
    ViewList* views = &frame->vl;
    BufferList* buffers = &frame->bl;
    View* view = get_active_view(views);
    Buffer* buf = view->buffer;
    if (ev->type & EVENT_CHAR) {
        buf_insert_char_at_cursor(buf, ev->ch, &view->cursor);
        buffer_modified(buf, views);
    }
    if (ev->type & EVENT_KEYDOWN) {
        switch (ev->keysym) {
        case EKEY_RIGHT:
            cur_next_char(&view->cursor);
            break;
        case EKEY_LEFT:
            cur_prev_char(&view->cursor);
            break;
        case EKEY_UP:
            cur_up_line(&view->cursor);
            break;
        case EKEY_DOWN:
            cur_down_line(&view->cursor);
            break;
        case EKEY_HOME:
            cur_start_line(&view->cursor);
            break;
        case EKEY_END:
            cur_end_line(&view->cursor);
            break;
        case EKEY_BACKSPACE: {
            TmpCursor where_to_remove = view->cursor;
            cur_prev_char(&where_to_remove);
            if (!cursor_eq(&view->cursor, &where_to_remove)) {
                buf_remove_range(buf, where_to_remove, where_to_remove);
                buffer_modified(buf, views);
            }
        } break;
        case EKEY_DEL:
            if (cur_has_char(&view->cursor)) {
                buf_remove_range(buf, view->cursor, view->cursor);
                buffer_modified(buf, views);
            }
            break;
        case EKEY_PAGEUP: {
            u32 lines_to_scroll = 1;
            for (u32 i = 0; i < lines_to_scroll; i++) {
                if (view->offset_y > 0) {
                    cur_up_line(&view->start_cursor);
                    view->offset_y--;
                }
            }
        } break;
        case EKEY_PAGEDOWN: {
            u32 lines_to_scroll = 1;
            for (u32 i = 0; i < lines_to_scroll; i++) {
                cur_down_line(&view->start_cursor);
                view->offset_y++;
            }
        } break;
        case EKEY_ESCAPE: {
            Buffer* minibuf = new_buffer_empty(&buffers, new_mem_default());
            View* old_view = view;
            new_view_into_buffer(views, minibuf);
            view = get_active_view(views);
            buf = view->buffer;
        } break;
        }
    }
}

public void
render_frame(const Frame* w) {
    switch (w->type) {
    case FRAME_EDIT:
        //render_edit_frame(&w->edit.vl);
        break;
    case FRAME_SAVE:
        //render_save_frame(w->save.buf);
        break;
    }
}

private void
update_current_frame(FrameList* wl, const Event* ev) {
    Frame* w = &wl->frames.first->obj;
    switch (w->type) {
    case FRAME_EDIT:
        update_edit_frame(wl, ev);
        break;
    case FRAME_SAVE:
        update_save_frame(wl, ev);
        break;
    }
}

// This function is directly called by the real main function found in
// the os directory.
public int
editor_main(int argc, char** argv) {
#ifdef HAVE_GUI
    const bool use_gui = true;
#else
    const bool use_gui = false;
#endif

    if (!ui_init(use_gui)) {
        ERROR("Program could not start");
        return 1;
    }

    FrameList frames = {0};
    Frame edit_frame_storage = {
        .type = FRAME_EDIT,
        .edit = {
            .vl = {0},
            .bl = {0},
        },
    };
    set_fg_frame(&frames, edit_frame_storage);
    struct EditFrame* edit_frame = &get_active_frame(&frames)->edit;
    BufferList* buffers = &edit_frame->bl;
    ViewList* views = &edit_frame->vl;

    Buffer* buf1;
    if (argc == 2) {
        buf1 = new_buffer_from_file(buffers, new_mem_default(), argv[1]);
    } else {
        buf1 = new_buffer_empty(buffers, new_mem_default());
    }
    new_view_into_buffer(views, buf1);

    // Main loop
    for (;;) {
        Event ev = ui_read_input(use_gui);

        update_current_frame(&frames, &ev);
        ui_render_everything(use_gui, &frames);  // TODO: Don't need to.

        if (ev.type & EVENT_QUIT) {
            break;
        }
        if (ev.type & EVENT_RENDER) {
            ui_render_everything(use_gui, &frames);
        }
        if (ev.type & EVENT_RESIZE) {
            //frame_resized(views, ev.width, ev.height);
        }
    }

    return 0;
}
