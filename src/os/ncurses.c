#include <ncurses.h>

public Result
tui_init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    raw();
    return true;
}

public void
tui_cleanup() {
    endwin();
}

private int
to_key(int k) {
    switch (k) {
    case KEY_HOME:
        return EKEY_HOME;
    case KEY_END:
        return EKEY_END;
    case KEY_DC:
        return EKEY_DEL;
    case KEY_BACKSPACE:
        return EKEY_BACKSPACE;
    case KEY_ENTER:
        return '\n';
    case KEY_DOWN:
        return EKEY_DOWN;
    case KEY_UP:
        return EKEY_UP;
    case KEY_LEFT:
        return EKEY_LEFT;
    case KEY_RIGHT:
        return EKEY_RIGHT;
    case 27:
        return EKEY_ESCAPE;
    case KEY_NPAGE:
        return EKEY_PAGEDOWN;
    case KEY_PPAGE:
        return EKEY_PAGEUP;
    default:
        return k;
    }
}

public Event
tui_read_input() {
    int ch = getch();
    if (key_is_printable(ch)) {
        return (Event) {
            .type = EVENT_KEYDOWN | EVENT_CHAR,
            .ch = ch,
            .keysym = to_key(ch),
        };
    } else {
        return (Event) {
            .type = EVENT_KEYDOWN,
            .keysym = to_key(ch),
        };
    }
}

public void
render_buffer_view(const View* bv, u32 start_col, u32 start_row,
                   u32 width, u32 height) {
    const u32 linenr_width = 4;         // 4 digits
    const u32 left_margin = linenr_width + 1;
    u32 row = start_row;                // Absolute row in window.
    u32 col = start_col + left_margin;  // Absolute column in window.
    u32 vis_lines = 0;                  // Number of visual lines printed.
    u32 linenr = bv->offset_y + 1;
    u32 line_started_at_row = row;
    int cursor_row = 0;
    int cursor_col = 0;

    TmpCursor cur = view_cursor_at_start(bv);
    for (; cur_has_char(&cur); cur_next_char(&cur)) {
        char c = bufpos_get_char(&cur.pos);
        bool hard_break = c == '\n';
        bool line_break = hard_break || col - start_col >= width;

        if (cursor_eq(&bv->cursor, &cur)) {
            cursor_col = col;
            cursor_row = row;
        }
        if (hard_break) {
            char nr[11];
            snprintf(nr, 11, "% *d ", linenr_width, linenr);
            u32 c = start_col;
            for (size_t j = 0; j < 5; j++) {
                mvaddch(line_started_at_row, c, nr[j]);
                c++;
            }
        }
        if (line_break) {
            vis_lines++;
            if (vis_lines >= height) {
                break;
            }
            row++;
            col = start_col + left_margin;
        }
        if (c != '\n') {
            mvaddch(row, col, c);
            col++;
        }
        if (hard_break) {
            linenr++;
            line_started_at_row = row;
        }
    }
    if (cursor_eq(&cur, &bv->cursor)) {
        cursor_col = col;
        cursor_row = row;
    }
    move(cursor_row, cursor_col);
}

#if 0
public void
render_buffer_view(const View* bv, u32 start_col, u32 start_row,
                   u32 width, u32 height) {
    int i;
    u32 skipped_lines;
    for (i = 0, skipped_lines = 0;
         i < buffer.data.len() && skipped_lines < buffer.scroll;
         i++)
    {
        i += get_remaining_line({i, 0}, buffer);
        skipped_lines++;
    }
    int line = skipped_lines + 1;
    int row = start_row;
    int col = start_col;
    int cursor_row = 0;
    int cursor_col = 0;
    bool line_start = true;
    for (; i < buffer.data.len(); i++) {
        char c = buffer.data.char_at(i);
        if (line_start) {
            char nr[11] = {0};
            snprintf(nr, 10, "% 4d ", line);
            for (size_t j = 0; j < 5; j++) {
                mvaddch(row, col, nr[j]);
                col++;
                line_start = false;
            }
        }
        if (buffer.cursor.pos == i) {
            cursor_col = col;
            cursor_row = row;
        }
        if (c == '\n') {
            row++;
            line++;
            col = start_col;
            line_start = true;
            continue;
        }
        mvaddch(row, col, c);
        col++;
        line_start = false;
    }
    if (buffer.cursor.pos == i) {
        if (line_start) {
            col += 5;
        }
        cursor_row = row;
        cursor_col = col;
    }
    move(cursor_row, cursor_col);
}
#endif

public void
render_edit_frame(const ViewList* vl) {
    for (const ListNode(View)* v = vl->views.first; v; v = v->next) {
        render_buffer_view(&v->obj, 0, 0, 50, 20);
    }
}

public void
render_save_frame() {
}

public void
tui_render_everything(const FrameList* frames) {
    clear();
    render_frame(&frames->frames.first->obj);
}
