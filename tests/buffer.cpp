#include "buffer.hpp"
#include "memory.hpp"

int
test_buffer_insert_iterate() {
    Buffer b = Buffer::new_empty(MemoryManager::new_default());
    b.insert_char('H', 0);
    assert(b.len() == 1);
    assert(b.char_at(0) == 'H');

    b.insert_char('i', 1);
    assert(b.len() == 2);
    assert(b.char_at(1) == 'i');

    b.insert_char('!', 2);
    assert(b.len() == 3);
    assert(b.char_at(2) == '!');

    assert(b.data.segments.len() == 1);

    b.insert_char('a', 1);
    assert(b.len() == 4);
    assert(b.data.segments.len() == 3);

    b.insert_char('O', 0);
    assert(b.len() == 5);
    assert(b.data.segments.len() == 4);

    b.insert_char('?', 5);
    assert(b.len() == 6);
    assert(b.data.segments.len() == 5);

    assert(b.char_at(0) == 'O');
    assert(b.char_at(1) == 'H');
    assert(b.char_at(2) == 'a');
    assert(b.char_at(3) == 'i');
    assert(b.char_at(4) == '!');
    assert(b.char_at(5) == '?');

    {
        Buffer::TmpCursor cur = b.index_to_cursor(0);
        assert(cur.get_char() == 'O');
        cur.next_char();
        assert(cur.has_char());
        assert(cur.get_char() == 'H');
        cur.next_char();
        assert(cur.has_char());
        assert(cur.get_char() == 'a');
        cur.next_char();
        assert(cur.has_char());
        assert(cur.get_char() == 'i');
        cur.next_char();
        assert(cur.has_char());
        assert(cur.get_char() == '!');
        cur.next_char();
        assert(cur.has_char());
        assert(cur.get_char() == '?');
        cur.next_char();
        assert(!cur.has_char());
    }
    {
        Buffer::TmpCursor cur = b.index_to_cursor(5);
        assert(cur.get_char() == '?');
        cur.prev_char();
        assert(cur.has_char());
        assert(cur.get_char() == '!');
        cur.prev_char();
        assert(cur.has_char());
        assert(cur.get_char() == 'i');
        cur.prev_char();
        assert(cur.has_char());
        assert(cur.get_char() == 'a');
        cur.prev_char();
        assert(cur.has_char());
        assert(cur.get_char() == 'H');
        cur.prev_char();
        assert(cur.has_char());
        assert(cur.get_char() == 'O');
        cur.prev_char();
        assert(cur.has_char());
        assert(cur.get_char() == 'O');
    }

    return 0;
}

int
test_buffer_remove() {
    Buffer b = Buffer::new_empty(MemoryManager::new_default());
    b.insert_char('a', 0);
    b.insert_char('b', 1);
    b.insert_char('c', 2);
    b.insert_char('X', 2);
    b.remove_range(b.index_to_cursor(0), b.index_to_cursor(3));
    assert(b.len() == 0);
    assert(b.data.segments.len() == 0);

    b.insert_char('x', 0);
    b.insert_char('y', 1);
    b.insert_char('z', 2);
    b.remove_range(b.index_to_cursor(1), b.index_to_cursor(1));
    assert(b.len() == 2);
    assert(b.char_at(0) == 'x');
    assert(b.char_at(1) == 'z');
    assert(b.data.segments.len() == 2);
    b.remove_range(b.index_to_cursor(1), b.index_to_cursor(1));
    assert(b.len() == 1);
    assert(b.char_at(0) == 'x');
    assert(b.data.segments.len() == 1);
    b.remove_range(b.index_to_cursor(0), b.index_to_cursor(0));
    assert(b.len() == 0);
    assert(b.data.segments.len() == 0);

    b.insert_char('0', 0);
    b.insert_char('1', 1);
    b.insert_char('2', 2);
    b.insert_char('3', 3);
    b.insert_char('Q', 2);
    b.remove_range(b.index_to_cursor(1), b.index_to_cursor(3));
    assert(b.len() == 2);
    assert(b.char_at(0) == '0');
    assert(b.char_at(1) == '3');
    assert(b.data.segments.len() == 2);

    return 0;
}
