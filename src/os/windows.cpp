#include "../common.hpp"

#include <windows.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


////////////////////////////////////////
// Memory functions - used by memory.cpp

void*
os_map_memory(usize size) {
    return malloc(size);
}

void
os_unmap_memory(void* addr, usize size) {
    free(addr);
}


////////////////////////////////////////
// Other functions

void
os_write_err(const char* str) {
    usize len = 0;
    for (usize i = 0; str[i] != '\0'; i++) {
        len++;
    }
    write(STDERR_FILENO, str, len);
}

void
os_abort() {
    abort();
}

struct Color {
    u8 r, g, b;
};

Color bg = {0x22, 0x22, 0x22};
Color fg = {0xff, 0xff, 0xff};
Color soft = {0x99, 0x99, 0x99};
u8 fontw = 10;
u8 fonth = 20;
HDC hdc;
HBRUSH brush;
HPEN pen;
HFONT font;

static int to_key(int k) {
    switch (k) {
    case VK_LEFT:
        return Key::LEFT;
    case VK_UP:
        return Key::UP;
    case VK_DOWN:
        return Key::DOWN;
    case VK_RIGHT:
        return Key::RIGHT;
    case VK_RETURN:
        return Key::RETURN;
    case VK_HOME:
        return Key::HOME;
    case VK_END:
        return Key::END;
    case VK_BACK:
        return Key::BACKSPACE;
    case VK_DELETE:
        return Key::DEL;
    case VK_ESCAPE:
        return Key::ESCAPE;
    case VK_PRIOR:
        return Key::PAGEUP;
    case VK_NEXT:
        return Key::PAGEDOWN;
    default:
        return 0;
    }
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                   LPARAM lParam) {
    switch (uMsg) {
    case WM_SIZE: {
    } break;
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_ACTIVATEAPP:
        break;
    case WM_PAINT: {
        RECT full_rect;
        GetClientRect(hWnd, &full_rect);
        ValidateRect(hWnd, &full_rect);
    } break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

#ifndef ENTRY
#define ENTRY editor_main
#endif
int CALLBACK
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wclass = {};
    wclass.style = CS_OWNDC;
    wclass.lpfnWndProc = WindowProc;
    wclass.hInstance = hInstance;
    wclass.lpszClassName = "eWindowClass";
    RegisterClass(&wclass);

    HWND window = CreateWindowEx(0, wclass.lpszClassName, "e",
                                 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                 CW_USEDEFAULT, CW_USEDEFAULT, 80 * fontw,
                                 25 * fonth, 0, 0, hInstance, 0);

    hdc = GetDC(window);
    brush = CreateSolidBrush(RGB(bg.r, bg.g, bg.b));
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));

    font = CreateFontA(fonth * 0.9, fontw * 0.9, 0, 0, FW_NORMAL, false,
                       false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
                       "Consolas");
    SelectObject(hdc, font);

    int ENTRY(int, char**);
    return ENTRY(__argc, __argv);
}
