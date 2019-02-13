#include "src/buffer.cpp"
#ifndef TEST
#include "src/editor.cpp"
#endif
#include "src/list.cpp"
#include "src/memory.cpp"
#include "src/view.cpp"
#include "src/os/linux.cpp"
#ifdef HAVE_X11
#include "src/os/linux-ui.cpp"
#endif

#ifdef TEST
#include "tests/main.cpp"
#include "tests/buffer.cpp"
#include "tests/list.cpp"
#include "tests/memory.cpp"
#include "tests/view.cpp"
#endif
