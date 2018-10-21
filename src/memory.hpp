#pragma once

#include "common.hpp"


struct MemoryManager {
    static MemoryManager
    new_default();

    void*
    alloc_at(void* addr, usize size);

    bool
    can_alloc_at(void* addr, usize size) const;

    void*
    alloc(usize size);

    template <typename T>
    T*
    alloc() {
        return static_cast<T*>(this->alloc(sizeof (T)));
    }

    void
    free_all();

    usize
    count_segments() const;


    // Internal

    void* last_segment;

    MemoryManager() = delete;
};
