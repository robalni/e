#include <windows.h>
#include <stdio.h>


extern Color bg;
extern Color fg;
extern Color soft;
extern u8 fontw;
extern u8 fonth;
extern HDC hdc;
extern HBRUSH brush;
extern HPEN pen;
extern HFONT font;

Result
init_gui() {
    return true;
}

static void
draw_char(char c, HDC hdc, int col, int row, Color color) {
    int x = col * fontw + 1;
    int y = row * fonth + 1;
    RECT rect = {x, y, x + fontw, y + fontw};
    SetBkMode(hdc, TRANSPARENT);
    // DrawText(hdc, &c, 1, &rect, DT_CENTER | DT_VCENTER);
    TextOut(hdc, x, y, &c, 1);
}

static void
draw_rect(HDC hdc, int col, int row, int w, int h,
                      Color color) {
    SetDCBrushColor(hdc, RGB(color.r, color.g, color.b));
    Rectangle(hdc, col * fontw, row * fonth, (col + w) * fontw,
              (row + h) * fonth + 1);
}

static void
draw_vline(HDC hdc, int col, int row, int count, Color color) {
    SetDCBrushColor(hdc, RGB(color.r, color.g, color.b));
    Rectangle(hdc, col * fontw - 1, row * fonth + 1, col * fontw + 2,
              (row + count) * fonth - 1);
}

static void
draw_buffer(const View& bv, HDC hdc, int start_col, int start_row) {
    int win_w = 80*fontw, win_h = 25*fonth;  // TODO: The real thing.
    // The background.
    draw_rect(hdc, start_col, start_row, win_w / fontw + 1, win_h / fonth + 1, {17, 17, 17});

    Buffer::TmpCursor cur = bv.cursor_at_start();
    int row = start_row;
    int col = start_col;
    int line = 1;
    bool line_start = true;
    for (; cur.has_char(); cur.next_char()) {
        char c = cur.get_char();
        if (line_start) {
            char nr[11] = {0};
            snprintf(nr, 10, "% 4d ", line);
            for (size_t j = 0; j < 5; j++) {
                draw_char(nr[j], hdc, col, row, soft);
                col++;
                line_start = false;
            }
            SetTextColor(hdc, RGB(fg.r, fg.g, fg.b));
        }
        //if (buffer.cursor.pos == i) {
        //    draw_vline(gc, col, row, 1, soft);
        //}
        if (c == '\n') {
            row++;
            line++;
            col = start_col;
            draw_rect(hdc, start_col, row, 80, 1, bg);
            line_start = true;
            continue;
        }
        draw_char(c, hdc, col, row, fg);
        col++;
        line_start = false;
    }
}

void
render_everything(const View& bv) {
    SelectObject(hdc, pen);
    SetBkColor(hdc, RGB(bg.r, bg.g, bg.b));
    SetTextColor(hdc, RGB(fg.r, fg.g, fg.b));
    draw_buffer(bv, hdc, 0, 0);
}

Event
read_input() {
    MSG msg;
    for (;;) {
        if (GetMessage(&msg, 0, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            switch (msg.message) {
            case WM_CHAR:
                return {Event::CHAR, msg.wParam};
            case WM_KEYDOWN:
                if (to_key(msg.wParam) != msg.wParam) {
                    return {Event::KEYDOWN, to_key(msg.wParam)};
                }
                break;
            }
        } else {
            exit(0);
        }
    }
    return {Event::UNKNOWN, 0};
}
