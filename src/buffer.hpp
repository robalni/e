#pragma once

#include "common.hpp"
#include "list.hpp"
#include "memory.hpp"

struct DataSegment;

struct Buffer {
    // Index into buffer (across segments) or into a segment.
    using Index = u32;

    // Only valid until the buffer data has been modified.  This is
    // because it points to a segment that can disappear or change.
    // It is normally used for iterating without changing anything.
    struct TmpCursor {
        List<DataSegment>::Node* segment;
        Index index;  // Index into the segment.

        char
        get_char() const;

        // Returns true if there is a char at the current position.
        bool
        has_char() const;

        // Updates the cursor to reference the next char.
        // Returns true if there was a next char.
        bool
        next_char();

        // Same as next_char but backwards.
        bool
        prev_char();
    };

    static Buffer
    new_empty(MemoryManager&& mem);

    // Inserts the character before index.
    void
    insert_char(char ch, Index index);

    void
    remove_range(TmpCursor first, TmpCursor last);

    char
    char_at(Index index) const;

    usize
    len() const;

    TmpCursor
    index_to_cursor(Index index) const;


    // Internal

    MemoryManager mem;

    struct BufferData {
        List<DataSegment> segments;
        DataSegment* last_written_segment;
    };
    BufferData data;

    Buffer() = delete;
};
