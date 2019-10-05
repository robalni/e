public int
test_memory_alloc() {
    Memory mem = new_mem_default();
    u32 n1 = 2981, n2 = 2190, n3 = 1337;
    u8* data1 = (u8*)mem_alloc_size(&mem, n1);
    u8* data2 = (u8*)mem_alloc_size(&mem, n2);
    u8* data3 = (u8*)mem_alloc_size(&mem, n3);

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

    assert(mem_count_segments(&mem) == 2);
    mem_free_all(&mem);
    assert(mem_count_segments(&mem) == 0);

    return 0;
}

public int
test_memory_append() {
    Memory mem = new_mem_default();

    u8* data_start = null;
    u32 i = 0;
    u8* next_addr = null;
    while ((next_addr = (u8*)mem_alloc_at(&mem, next_addr, 1))) {
        if (!data_start) {
            data_start = next_addr;
        }
        *next_addr = i;
        i++;
        next_addr++;
    }

    assert(mem_count_segments(&mem) == 1);
    assert(i > 5);

    for (u32 j = 0; j < i; j++) {
        assert(data_start[j] == (j & 0xff));
    }

    return 0;
}
