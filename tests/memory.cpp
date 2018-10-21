#include "memory.hpp"
#include "common.hpp"


int
test_memory_alloc() {
    MemoryManager mem = MemoryManager::new_default();
    u32 n1 = 2981, n2 = 2190, n3 = 1337;
    u8* data1 = static_cast<u8*>(mem.alloc(n1));
    u8* data2 = static_cast<u8*>(mem.alloc(n2));
    u8* data3 = static_cast<u8*>(mem.alloc(n3));

    assert(data1 != null);
    assert(data2 != null);
    assert(data3 != null);

    for (u32 i = 0; i < n1; i++) {
        data1[i] = 'x';
    }
    for (u32 i = 0; i < n2; i++) {
        data2[i] = 'y';
    }
    for (u32 i = 0; i < n3; i++) {
        data3[i] = 'z';
    }

    for (u32 i = 0; i < n1; i++) {
        assert(data1[i] == 'x');
    }
    for (u32 i = 0; i < n2; i++) {
        assert(data2[i] == 'y');
    }
    for (u32 i = 0; i < n3; i++) {
        assert(data3[i] == 'z');
    }

    assert(mem.count_segments() == 2);
    mem.free_all();
    assert(mem.count_segments() == 0);

    return 0;
}

int
test_memory_append() {
    MemoryManager mem = MemoryManager::new_default();

    u8* data_start = null;
    u32 i = 0;
    u8* next_addr = null;
    while ((next_addr = static_cast<u8*>(mem.alloc_at(next_addr, 1)))) {
        if (!data_start) {
            data_start = next_addr;
        }
        *next_addr = i;
        i++;
        next_addr++;
    }

    assert(mem.count_segments() == 1);
    assert(i > 5);

    for (u32 j = 0; j < i; j++) {
        assert(data_start[j] == (j & 0xff));
    }

    return 0;
}
