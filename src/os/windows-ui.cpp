#include <windows.h>


Result
init_gui() {
    return true;
}

void
render_everything(const View& bv) {
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
                return {Event::KEYDOWN, msg.wParam};
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
