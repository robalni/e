#pragma once

using u8 = unsigned char;
using i8 = signed char;
using u16 = unsigned short;
using i16 = short;
using u32 = unsigned;
using i32 = int;
using u64 = unsigned long;
using i64 = long;
using usize = unsigned long;
using isize = long;
static_assert (sizeof (u8) == 1);
static_assert (sizeof (i8) == 1);
static_assert (sizeof (u16) == 2);
static_assert (sizeof (i16) == 2);
static_assert (sizeof (u32) == 4);
static_assert (sizeof (i32) == 4);
static_assert (sizeof (u64) == 8);
static_assert (sizeof (i64) == 8);
static_assert (sizeof (usize) == sizeof (void*));
static_assert (sizeof (isize) == sizeof (void*));

#define null 0

#define MAX(a, b)  \
    ((a) > (b) ? (a) : (b))

#define STR_(s) #s
#define STR(s) STR_(s)

void
os_abort();

void
os_write_err(const char* str);

#define assert(expr)  \
    do {  \
        if (!(expr)) {  \
            os_write_err("Assert error: " __FILE__ ":" STR(__LINE__) ": "  \
                         #expr "\n");  \
            os_abort();  \
        }  \
    } while (0)
