#include "src/buffer.cpp"
#ifndef TEST
#include "src/editor.cpp"
#endif
#include "src/list.cpp"
#include "src/memory.cpp"
#include "src/view.cpp"

#ifdef LINUX
#  include "src/os/linux.cpp"
#  ifdef HAVE_X11
#    include "src/os/linux-ui.cpp"
#  endif
#endif
#ifdef WINDOWS
#  include "src/os/windows.cpp"
#  include "src/os/windows-ui.cpp"
#endif

#ifdef TEST
#include "tests/main.cpp"
#include "tests/buffer.cpp"
#include "tests/list.cpp"
#include "tests/memory.cpp"
#include "tests/view.cpp"
#endif
