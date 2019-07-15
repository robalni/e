struct Memory {
    void* last_segment;
};
typedef struct Memory Memory;

public void*
os_map_memory(usize size);

public void
os_unmap_memory(void* addr, usize size);

#define DEFAULT_SEG_SIZE 0x1000

struct SegHeader {
    struct SegHeader* prev;
    // Size of segment (both allocated and unallocated memory).
    usize size;
    // The next unallocated space in this segment.
    void* next_free;
};
typedef struct SegHeader SegHeader;

private void
mem_header_set_all(SegHeader* self, SegHeader* prev, usize size, void* next_free) {
    self->prev = prev;
    self->size = size;
    self->next_free = next_free;
}

private SegHeader*
last_segheader(const Memory* mem) {
    return (SegHeader*)mem->last_segment;
}

private void*
ptr_add_bytes(void* ptr, usize bytes) {
    return (u8*)ptr + bytes;
}

// Are there SIZE free bytes at the end of the segment?
private bool
can_append(SegHeader* header, usize size) {
    return ptr_add_bytes(header->next_free, size)
        <= ptr_add_bytes(header, header->size);
}

// Creates a new segment and fills in its header.
private void*
create_segment(void* last_segment, usize minsize) {
    usize size = MAX(minsize, DEFAULT_SEG_SIZE);
    void* seg = os_map_memory(size);
    SegHeader* header = (SegHeader*)seg;
    SegHeader* last_header = (SegHeader*)last_segment;
    mem_header_set_all(header, last_header, size,
                       ptr_add_bytes(seg, sizeof *header));
    return seg;
}

public Memory
new_mem_default() {
    return (Memory) {
        .last_segment = create_segment(null, DEFAULT_SEG_SIZE),
    };
}

public bool
mem_can_alloc_at(const Memory* mem, void* addr, usize size) {
    SegHeader* last_header = last_segheader(mem);
    if (addr != last_header->next_free) {
        return false;
    }
    // Is there no more free memory in the last segment?
    if (!can_append(last_header, size)) {
        return false;
    }
    return true;
}

public void*
mem_alloc_at(Memory* mem, void* addr, usize size) {
    if (addr != null && !mem_can_alloc_at(mem, addr, size)) {
        return null;
    }
    SegHeader* last_header = last_segheader(mem);
    void* new_mem = last_header->next_free;
    last_header->next_free = ptr_add_bytes(new_mem, size);
    return new_mem;
}

#define mem_alloc(mem, T) mem_alloc_size(mem, sizeof (T))

public void*
mem_alloc_size(Memory* mem, usize size) {
    SegHeader* last_header = last_segheader(mem);
    void* new_mem = last_header->next_free;
    // Is there no more free memory in the last segment?
    if (!can_append(last_header, size)) {
        mem->last_segment = create_segment(last_header, size);
        last_header = last_segheader(mem);
        new_mem = last_header->next_free;
    }
    last_header->next_free = ptr_add_bytes(new_mem, size);
    return new_mem;
}

public void
mem_free_all(Memory* mem) {
    mem->last_segment = null;
    SegHeader* header = last_segheader(mem);
    while (header) {
        SegHeader* prev = header->prev;
        os_unmap_memory(header, header->size);
        header = prev;
    }
}

public usize
mem_count_segments(const Memory* mem) {
    SegHeader* header = last_segheader(mem);
    usize count = 0;
    while (header) {
        header = header->prev;
        count++;
    }
    return count;
}
