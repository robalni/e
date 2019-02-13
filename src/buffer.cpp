#include "buffer.hpp"


// No segment should have length 0 (except temporarily).  They should
// be removed instead.
struct DataSegment {
    char* start;
    usize len;
};


////////////////////////////////////////
// Private functions

// Splits SEG in two at SEG_INDEX and puts the second one in NEW_SEG.
// Returns NEW_SEG.
static DataSegment&
split_seg(DataSegment& seg, Buffer::Index seg_index, DataSegment& new_seg) {
    if (seg_index > seg.len) {
        seg_index = seg.len;
    }
    new_seg.start = seg.start + seg_index;
    new_seg.len = seg.len - seg_index;
    seg.len = seg_index;
    return new_seg;
}

static List<DataSegment>::Node*
insert_char_in_segment(Buffer& buffer, char ch, List<DataSegment>::Node* node,
                       Buffer::Index index) {
    using SegNode = List<DataSegment>::Node;
    DataSegment* segment = &node->obj;
    SegNode* second_node = buffer.mem.alloc<SegNode>();
    buffer.data.segments.insert(second_node, node->next);
    split_seg(*segment, index, second_node->obj);
    SegNode* new_node;
    if (second_node->obj.len == 0) {
        new_node = second_node;
    } else if (index == 0) {
        new_node = node;
    } else {
        new_node = buffer.mem.alloc<SegNode>();
        buffer.data.segments.insert(new_node, second_node);
    }
    char* chp = buffer.mem.alloc<char>();
    *chp = ch;
    new_node->obj = DataSegment {chp, 1};
    return new_node;
}


////////////////////////////////////////
// Public functions

Buffer
Buffer::new_empty(MemoryManager&& mem) {
    return Buffer {
        mem,
        BufferData {
            List<DataSegment>::new_empty(),
            null,
        },
    };
}

void
Buffer::insert_char(char ch, Index index) {
    using SegNode = List<DataSegment>::Node;
    if (this->data.segments.first == null) {
        SegNode* first_node = mem.alloc<SegNode>();
        this->data.segments.add_last(first_node);
        char* chp = mem.alloc<char>();
        *chp = ch;
        first_node->obj = DataSegment {chp, 1};
        this->data.last_written_segment = &first_node->obj;
        return;
    }
    DataSegment* latest_seg = this->data.last_written_segment;
    TmpCursor where = this->index_to_cursor(index);
    if (where.index == 0 && where.segment->prev != null) {
        where.segment = where.segment->prev;
        where.index = where.segment->obj.len;
    }
    void* to_write = null;
    // Are we appending the the same segment as last time?
    // Then we don't need to create a new segment!
    if (&where.segment->obj == latest_seg) {
        to_write
            = this->mem.alloc_at(where.segment->obj.start + where.index, 1);
        if (to_write != null) {
            where.segment->obj.len++;
        }
    }
    // We could not append.  Let's create a new segment.
    if (to_write == null) {
        SegNode* new_node
            = insert_char_in_segment(*this, ch, where.segment, where.index);
        to_write = new_node->obj.start;
        latest_seg = &new_node->obj;
    }

    // Write the character.
    char* char_to_write = static_cast<char*>(to_write);
    *char_to_write = ch;
    this->data.last_written_segment = latest_seg;
}

void
Buffer::remove_range(TmpCursor first, TmpCursor last) {
    using SegNode = List<DataSegment>::Node;
    SegNode* node = first.segment;
    // Look at each node and remove it or change the size of it.
    for (;;) {
        SegNode* next_node = node->next;
        bool chars_to_left
            = node == first.segment && first.index > 0;
        bool chars_to_right
            = node == last.segment && last.index < node->obj.len - 1;
        if (!chars_to_left && !chars_to_right) {
            this->data.segments.remove(node);
        } else if (chars_to_left && chars_to_right) {
            SegNode* right_node = this->mem.alloc<SegNode>();
            right_node->obj = DataSegment {
                node->obj.start + last.index + 1,
                node->obj.len - last.index - 1,
            };
            node->obj.len = first.index;
            this->data.segments.insert(right_node, node->next);
        } else {
            if (chars_to_left) {
                node->obj.len = first.index;
            }
            if (chars_to_right) {
                node->obj.start += last.index + 1;
                node->obj.len -= last.index + 1;
            }
        }
        if (node == last.segment) {
            break;
        }
        node = next_node;
    }
}

char
Buffer::char_at(Index index) const {
    auto node = this->data.segments.first;
    usize chars_passed = 0;
    while (node && chars_passed + node->obj.len <= index) {
        chars_passed += node->obj.len;
        node = node->next;
    }
    assert(node);
    return node->obj.start[index - chars_passed];
}

usize
Buffer::len() const {
    auto node = this->data.segments.first;
    usize count = 0;
    while (node) {
        count += node->obj.len;
        node = node->next;
    }
    return count;
}

Buffer::TmpCursor
Buffer::index_to_cursor(Index index) const {
    List<DataSegment>::Node* node = this->data.segments.first;
    Index len_sum = 0;
    while (node) {
        if (len_sum + node->obj.len > index || node->next == null) {
            break;
        }
        len_sum += node->obj.len;
        node = node->next;
    }

    return {node, index - len_sum};
}

char
Buffer::TmpCursor::get_char() const {
    return this->segment->obj.start[this->index];
}

bool
Buffer::TmpCursor::next_char() {
    this->index++;
    if (this->index >= this->segment->obj.len) {
        if (this->segment->next != null) {
            this->segment = this->segment->next;
            this->index = 0;
        } else {
            this->index--;
            return false;
        }
    }
    return true;
}

bool
Buffer::TmpCursor::prev_char() {
    if (this->index == 0) {
        if (this->segment->prev != null) {
            this->segment = this->segment->prev;
            this->index = this->segment->obj.len;
        } else {
            return false;
        }
    }
    this->index--;
    return true;
}
