#include "memory.hpp"
#include "common.hpp"


void*
os_map_memory(usize size);

void
os_unmap_memory(void* addr, usize size);


#define DEFAULT_SEG_SIZE 0x1000

struct SegHeader {
    SegHeader* prev;
    // Size of segment (both allocated and unallocated memory).
    usize size;
    // The next unallocated space in this segment.
    void* next_free;

    void
    set_all(SegHeader* prev, usize size, void* next_free) {
        this->prev = prev;
        this->size = size;
        this->next_free = next_free;
    }
};


////////////////////////////////////////
// Private functions

static SegHeader*
last_segheader(const MemoryManager* mem) {
    return static_cast<SegHeader*>(mem->last_segment);
}

static void*
ptr_add_bytes(void* ptr, usize bytes) {
    return static_cast<u8*>(ptr) + bytes;
}

// Are there SIZE free bytes at the end of the segment?
static bool
can_append(SegHeader* header, usize size) {
    return ptr_add_bytes(header->next_free, size)
        <= ptr_add_bytes(header, header->size);
}

// Creates a new segment and fills in its header.
static void*
create_segment(void* last_segment, usize minsize) {
    usize size = MAX(minsize, DEFAULT_SEG_SIZE);
    void* seg = os_map_memory(size);
    SegHeader* header = static_cast<SegHeader*>(seg);
    SegHeader* last_header = static_cast<SegHeader*>(last_segment);
    header->set_all(last_header, size, ptr_add_bytes(seg, sizeof *header));
    return seg;
}


////////////////////////////////////////
// Public functions

MemoryManager
MemoryManager::new_default() {
    return {
        create_segment(null, DEFAULT_SEG_SIZE),
    };
}

void*
MemoryManager::alloc_at(void* addr, usize size) {
    if (addr != null && !this->can_alloc_at(addr, size)) {
        return null;
    }
    SegHeader* last_header = last_segheader(this);
    void* new_mem = last_header->next_free;
    last_header->next_free = ptr_add_bytes(new_mem, size);
    return new_mem;
}

bool
MemoryManager::can_alloc_at(void* addr, usize size) const {
    SegHeader* last_header = last_segheader(this);
    if (addr != last_header->next_free) {
        return false;
    }
    // Is there no more free memory in the last segment?
    if (!can_append(last_header, size)) {
        return false;
    }
    return true;
}

void*
MemoryManager::alloc(usize size) {
    SegHeader* last_header = last_segheader(this);
    void* new_mem = last_header->next_free;
    // Is there no more free memory in the last segment?
    if (!can_append(last_header, size)) {
        this->last_segment = create_segment(last_header, size);
        last_header = last_segheader(this);
        new_mem = last_header->next_free;
    }
    last_header->next_free = ptr_add_bytes(new_mem, size);
    return new_mem;
}

void
MemoryManager::free_all() {
    SegHeader* header = last_segheader(this);
    while (header) {
        SegHeader* prev = header->prev;
        os_unmap_memory(header, header->size);
        header = prev;
    }
    this->last_segment = null;
}

usize
MemoryManager::count_segments() const {
    SegHeader* header = last_segheader(this);
    usize count = 0;
    while (header) {
        header = header->prev;
        count++;
    }
    return count;
}
