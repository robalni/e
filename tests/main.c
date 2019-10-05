#include "test-buffer.c"
#include "test-list.c"
#include "test-memory.c"
#include "test-view.c"

#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void
exit(int);

typedef int (*TestFn)();

static inline int
run_test(TestFn fn) {
    if (fork() == 0) {
        exit(fn());
    }
    int status;
    wait(&status);
    return status;
}

static inline void
print_test_result(const char* test_name, int status) {
    if (status == 0) {
        printf("  \x1b[92mOK\x1b[m    %s\n", test_name);
    } else {
        printf("  \x1b[91mFAIL\x1b[m  %-10s ", test_name);
        if (WEXITSTATUS(status)) {
            printf(" status:\x1b[91m%d\x1b[m ", WEXITSTATUS(status));
        }
        if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            printf(" signal:\x1b[91m%d\x1b[m (%s) ", sig, strsignal(sig));
        }
        puts("");
    }
}

#define test(name) {  \
    extern int test_##name();  \
    int status = run_test(test_##name);  \
    print_test_result(#name, status);  \
    if (status != 0) n_failed++;  \
}

static inline void
print_summary(int n_failed) {
    puts("");
    if (n_failed == 0) {
        printf("All tests \x1b[92msucceeded\x1b[m!\n");
    } else {
        printf("%d test%s \x1b[91mfailed\x1b[m!\n",
               n_failed, n_failed > 1 ? "s" : "");
    }
}

int
test_main(int argc, char** argv) {
    int n_failed = 0;
    puts("");

    test(memory_alloc);
    test(memory_append);
    test(list_insert);
    test(list_remove);
    test(buffer_insert_iterate);
    test(buffer_remove);
    test(view);

    print_summary(n_failed);
    return n_failed;
}
