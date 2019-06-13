#pragma once

#include <stdint.h>

using u8 = uint8_t;
using i8 = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;
using usize = size_t;
using isize = ssize_t;

#define null nullptr

#define MAX(a, b)  \
    ((a) > (b) ? (a) : (b))

#define STR_(s) #s
#define STR(s) STR_(s)

#define ERROR(msg)  \
    os_write_err(msg " (" __FILE__ ":" STR(__LINE__) ")\n")

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

struct [[nodiscard]] Result {
    bool is_ok;

    Result(bool ok)
        : is_ok(ok)
    {}

    operator bool() const {
        return is_ok;
    }
};
