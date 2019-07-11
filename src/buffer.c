// Index into buffer (across segments) or into a segment.
typedef u32 Index;

// No segment should have length 0 (except temporarily).  They should
// be removed instead.
struct DataSegment {
    char* start;
    usize len;

    // Incremented when this segment is changed in a way that requires
    // cursors to be renewed.  If a cursor in this segment has a
    // revision less than this then it needs to be renewed.
    u64 revision;
};
typedef struct DataSegment DataSegment;
make_list_type(DataSegment);

struct BufferData {
    List(DataSegment) segments;
    DataSegment* last_written_segment;
};

struct Buffer {
    Memory mem;

    struct BufferData data;

    // Incremented every time the buffer is changed in a way so that
    // we have to renew its cursors.  Used to see whether a cursor is
    // still usable.
    u64 cursor_revision;

    struct LatestChange {
        Index where;
        i32 chars_added;
    } latest_change;
};
typedef struct Buffer Buffer;
make_list_type(Buffer);

// Only valid until the buffer data has been modified.  This is
// because it points to a segment that can disappear or change.
// It is normally used for iterating without changing anything.
struct TmpCursor {
    ListNode(DataSegment)* segment;
    Index index;  // Index into the segment.
    u64 revision;
    Index full_backup_index;  // Only used to renew cursor.
};
typedef struct TmpCursor TmpCursor;

typedef List(Buffer) BufferList;

// Splits SEG in two at SEG_INDEX and puts the second one in NEW_SEG.
// Returns NEW_SEG.
private DataSegment*
split_seg(const Buffer* buf, DataSegment* seg, Index seg_index,
          DataSegment* new_seg) {
    assert(seg);
    assert(new_seg);
    if (seg_index > seg->len) {
        seg_index = seg->len;
    }
    new_seg->start = seg->start + seg_index;
    new_seg->len = seg->len - seg_index;
    if (seg_index < seg->len) {
        seg->revision++;
    }
    seg->len = seg_index;
    return new_seg;
}

// Splits NODE in two (if inserting at the beginning or end) or three
// (if inserting in the middle) and puts CH in the newly allocated
// segment that does not contain any data yet.  Returns the node that
// contains the new segment with CH.
private ListNode(DataSegment)*
insert_char_in_segment(Buffer* buffer, char ch, ListNode(DataSegment)* node,
                       Index index) {
    assert(buffer);
    typedef ListNode(DataSegment) SegNode;

    DataSegment* segment = &node->obj;
    SegNode* second_node = mem_alloc(&buffer->mem, SegNode);
    list_insert(&buffer->data.segments, second_node, node->next);
    split_seg(buffer, segment, index, &second_node->obj);
    SegNode* new_node;
    if (second_node->obj.len == 0) {
        new_node = second_node;
    } else if (index == 0) {
        new_node = node;
    } else {
        new_node = mem_alloc(&buffer->mem, SegNode);
        list_insert(&buffer->data.segments, new_node, second_node);
    }
    char* chp = mem_alloc(&buffer->mem, char);
    *chp = ch;
    new_node->obj = (DataSegment) {
        .start = chp,
        .len = 1,
        .revision = segment->revision,
    };
    return new_node;
}

public Buffer
new_buffer_empty(Memory mem) {
    return (Buffer) {
        .mem = mem,
        .data = (struct BufferData) {
            .segments = new_list_empty(DataSegment),
            .last_written_segment = null,
        },
        .cursor_revision = 0,
    };
}

public Buffer
new_buffer_from_file(Memory mem, const char* filename) {
    assert(filename);
    typedef ListNode(DataSegment) SegNode;

    List(DataSegment) segs = new_list_empty(DataSegment);
    SegNode* seg = mem_alloc(&mem, SegNode);
    list_add_last(&segs, seg);

    File file = os_open_file(filename);
    size_t file_size = os_file_size(file);
    seg->obj.start = mem_alloc_size(&mem, file_size);
    seg->obj.len = file_size;
    os_file_read_all(file, seg->obj.start, file_size);
    os_close_file(file);

    return (Buffer) {
        .mem = mem,
        .data = (struct BufferData) {
            .segments = segs,
            .last_written_segment = &seg->obj,
        },
        .cursor_revision = 0,
    };
}

public TmpCursor
buf_index_to_cursor_relative(const Buffer* buf, ListNode(DataSegment)* node,
                             Index index, Index full_index_of_node) {
    Index len_sum = 0;
    while (node) {
        if (len_sum + node->obj.len > index || node->next == null) {
            break;
        }
        len_sum += node->obj.len;
        node = node->next;
    }

    return (TmpCursor) {
        .segment = node,
        .index = index - len_sum,
        .revision = buf->cursor_revision,
        .full_backup_index = full_index_of_node + index,
    };
}

public TmpCursor
buf_index_to_cursor(const Buffer* buf, Index index) {
    assert(buf);
    ListNode(DataSegment)* node = buf->data.segments.first;
    return buf_index_to_cursor_relative(buf, node, index, 0);
}

// Inserts the character before the cursor.
public void
buf_insert_char_at_cursor(Buffer* buf, char ch, TmpCursor* cur) {
    assert(buf);
    assert(cur);
    typedef ListNode(DataSegment) SegNode;
    Index full_index_of_segment = cur->full_backup_index - cur->index;

    buf->cursor_revision++;
    buf->latest_change.where = cur->full_backup_index;
    buf->latest_change.chars_added = 1;

    if (buf->data.segments.first == null) {
        SegNode* first_node = mem_alloc(&buf->mem, SegNode);
        list_add_last(&buf->data.segments, first_node);
        char* chp = mem_alloc(&buf->mem, char);
        *chp = ch;
        first_node->obj = (DataSegment) {
            .start = chp,
            .len = 1,
            .revision = buf->cursor_revision,
        };
        buf->data.last_written_segment = &first_node->obj;
        *cur = buf_index_to_cursor(buf, 1);
        return;
    }
    DataSegment* latest_seg = buf->data.last_written_segment;
    if (cur->index == 0 && cur->segment->prev != null) {
        // This is just for use in this function.  The cursor will be
        // replaced later in this function.
        cur->segment = cur->segment->prev;
        cur->index = cur->segment->obj.len;
        full_index_of_segment -= cur->index;
    }
    void* to_write = null;
    // Are we appending the the same segment as last time?
    // Then we don't need to create a new segment!
    if (&cur->segment->obj == latest_seg) {
        to_write
            = mem_alloc_at(&buf->mem, cur->segment->obj.start + cur->index, 1);
        if (to_write != null) {
            cur->segment->obj.len++;
        }
    }
    // We could not append.  Let's create a new segment.
    if (to_write == null) {
        SegNode* new_node
            = insert_char_in_segment(buf, ch, cur->segment, cur->index);
        to_write = new_node->obj.start;
        latest_seg = &new_node->obj;
    }

    // Write the character.
    char* char_to_write = (char*)to_write;
    *char_to_write = ch;
    buf->data.last_written_segment = latest_seg;
    *cur = buf_index_to_cursor_relative(buf, cur->segment, cur->index + 1,
                                        full_index_of_segment);
}

// Inserts the character before index.
public void
buf_insert_char(Buffer* buf, char ch, Index index) {
    TmpCursor cur = buf_index_to_cursor(buf, index);
    buf_insert_char_at_cursor(buf, ch, &cur);
}

private void
buf_remove_range(Buffer* buf, TmpCursor first, TmpCursor last) {
    assert(buf);
    typedef ListNode(DataSegment) SegNode;
    SegNode* node = first.segment;

    buf->cursor_revision++;
    buf->latest_change.where = first.full_backup_index;
    buf->latest_change.chars_added
        = -(last.full_backup_index - first.full_backup_index + 1);

    // Look at each node and remove it or change the size of it.
    for (;;) {
        SegNode* next_node = node->next;
        bool chars_to_left
            = node == first.segment && first.index > 0;
        bool chars_to_right
            = node == last.segment && last.index < node->obj.len - 1;
        if (!chars_to_left && !chars_to_right) {
            list_remove(&buf->data.segments, node);
            node->obj.revision = buf->cursor_revision;
        } else if (chars_to_left && chars_to_right) {
            node->obj.revision = buf->cursor_revision;
            SegNode* right_node = mem_alloc(&buf->mem, SegNode);
            right_node->obj = (DataSegment) {
                .start = node->obj.start + last.index + 1,
                .len = node->obj.len - last.index - 1,
            };
            node->obj.len = first.index;
            list_insert(&buf->data.segments, right_node, node->next);
        } else {
            if (chars_to_left) {
                node->obj.revision = buf->cursor_revision;
                node->obj.len = first.index;
            }
            if (chars_to_right) {
                node->obj.start += last.index + 1;
                node->obj.len -= last.index + 1;
                node->obj.revision = buf->cursor_revision;
            }
        }
        if (node == last.segment) {
            break;
        }
        node = next_node;
    }
}

private usize
buf_len(const Buffer* buf) {
    const ListNode(DataSegment)* node = buf->data.segments.first;
    usize count = 0;
    while (node) {
        count += node->obj.len;
        node = node->next;
    }
    return count;
}

// Returns cursor pointing to the first character in this buffer.
public TmpCursor
buf_cursor_at_start(const Buffer* buf) {
    assert(buf);
    return (TmpCursor) {
        .segment = buf->data.segments.first,
        .index = 0,
        .revision = buf->cursor_revision,
        .full_backup_index = 0,
    };
}

public char
cur_get_char(const TmpCursor* cur) {
    assert(cur);
    assert(cur->index < cur->segment->obj.len);
    return cur->segment->obj.start[cur->index];
}

// Returns true if there is a char at the current position.
public bool
cur_has_char(const TmpCursor* cur) {
    assert(cur);
    return cur->segment && cur->segment->obj.len > cur->index;
}

// Updates the cursor to reference the next char.
public void
cur_next_char(TmpCursor* cur) {
    assert(cur);
    if (cur->segment) {
        cur->index++;
        cur->full_backup_index++;
        // If this is last segment (segment->next is null) then we should
        // go out of bounds to mark that we reached EOF.
        if (cur->index >= cur->segment->obj.len) {
            if (cur->segment->next) {
                cur->segment = cur->segment->next;
                cur->index = 0;
            } else if (cur->index > cur->segment->obj.len) {
                cur->index--;
                cur->full_backup_index--;
            }
        }
    }
}

// Same as cur_next_char but backwards.
public void
cur_prev_char(TmpCursor* cur) {
    assert(cur);
    if (cur->index > 0) {
        cur->index--;
        cur->full_backup_index--;
    } else {
        if (cur->segment && cur->segment->prev) {
            cur->segment = cur->segment->prev;
            cur->index = cur->segment->obj.len - 1;
            cur->full_backup_index--;
        }
    }
}

// Two TmpCursor are equal if they point to the same segment
// and have the same index.
public bool
cursor_eq(const TmpCursor* a, const TmpCursor* b) {
    return a->segment == b->segment
        && a->index == b->index;
}
