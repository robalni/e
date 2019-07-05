#include <stdint.h>
#include <stdlib.h>

typedef enum {false, true} bool;

typedef  uint8_t  u8;
typedef   int8_t  i8;
typedef uint16_t u16;
typedef  int16_t i16;
typedef uint32_t u32;
typedef  int32_t i32;
typedef uint64_t u64;
typedef  int64_t i64;
typedef   size_t usize;
typedef  ssize_t isize;

// Should be used to mark whether a function is meant to be used in
// other files.  This can be enforced by scripts.
#define public static
#define private static

#define null NULL

#define MAX(a, b)  \
    ((a) > (b) ? (a) : (b))

#define STR_(s) #s
#define STR(s) STR_(s)

#define ERROR(msg)  \
    os_write_err(msg " (" __FILE__ ":" STR(__LINE__) ")\n")

static void
os_abort();

static void
os_write_err(const char* str);

#define assert(expr)  \
    do {  \
        if (!(expr)) {  \
            os_write_err("Assert error: " __FILE__ ":" STR(__LINE__) ": "  \
                         #expr "\n");  \
            os_abort();  \
        }  \
    } while (0)

typedef bool Result;
