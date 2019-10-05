private bool
buf_content_eq(const Buffer* buf, const char* wanted) {
    char content[100];
    usize wanted_len = strlen(wanted);
    assert(wanted_len < sizeof content);
    buf_get_content(buf, content, sizeof content);
    return strcmp(content, wanted) == 0;
}

public int
test_buffer_insert_iterate() {
    Buffer* buf = new_buffer_empty(null, new_mem_default());

    buf_insert_char(buf, 'H', 0);
    assert(buf_len(buf) == 1);
    assert(buf_content_eq(buf, "H"));

    buf_insert_char(buf, 'i', 1);
    assert(buf_len(buf) == 2);
    assert(buf_content_eq(buf, "Hi"));

    buf_insert_char(buf, '!', 2);
    assert(buf_len(buf) == 3);
    assert(buf_content_eq(buf, "Hi!"));

    assert(list_len(&buf->data.segments) == 1);

    buf_insert_char(buf, 'a', 1);
    assert(buf_len(buf) == 4);
    assert(list_len(&buf->data.segments) == 3);

    buf_insert_char(buf, 'O', 0);
    assert(buf_len(buf) == 5);
    assert(list_len(&buf->data.segments) == 4);

    buf_insert_char(buf, '?', 5);
    assert(buf_len(buf) == 6);
    assert(list_len(&buf->data.segments) == 5);

    assert(buf_content_eq(buf, "OHai!?"));

    {
        TmpCursor cur = buf_index_to_cursor(buf, 0);
        assert(bufpos_get_char(&cur.pos) == 'O');
        cur_next_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == 'H');
        cur_next_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == 'a');
        cur_next_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == 'i');
        cur_next_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == '!');
        cur_next_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == '?');
        cur_next_char(&cur);
        assert(!cur_has_char(&cur));
    }
    {
        TmpCursor cur = buf_index_to_cursor(buf, 5);
        assert(bufpos_get_char(&cur.pos) == '?');
        cur_prev_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == '!');
        cur_prev_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == 'i');
        cur_prev_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == 'a');
        cur_prev_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == 'H');
        cur_prev_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == 'O');
        cur_prev_char(&cur);
        assert(cur_has_char(&cur));
        assert(bufpos_get_char(&cur.pos) == 'O');
    }

    return 0;
}

public int
test_buffer_remove() {
    Buffer* buf = new_buffer_empty(null, new_mem_default());

    buf_insert_char(buf, 'a', 0);
    buf_insert_char(buf, 'b', 1);
    buf_insert_char(buf, 'c', 2);
    buf_insert_char(buf, 'X', 2);
    buf_remove_range(buf, buf_index_to_cursor(buf, 0),
                     buf_index_to_cursor(buf, 3));
    assert(buf_len(buf) == 0);
    assert(list_len(&buf->data.segments) == 0);

    buf_insert_char(buf, 'x', 0);
    buf_insert_char(buf, 'y', 1);
    buf_insert_char(buf, 'z', 2);
    buf_remove_range(buf, buf_index_to_cursor(buf, 1),
                     buf_index_to_cursor(buf, 1));
    assert(buf_len(buf) == 2);
    assert(buf_content_eq(buf, "xz"));
    assert(list_len(&buf->data.segments) == 2);
    buf_remove_range(buf, buf_index_to_cursor(buf, 1),
                     buf_index_to_cursor(buf, 1));
    assert(buf_len(buf) == 1);
    assert(buf_content_eq(buf, "x"));
    assert(list_len(&buf->data.segments) == 1);
    buf_remove_range(buf, buf_index_to_cursor(buf, 0),
                     buf_index_to_cursor(buf, 0));
    assert(buf_len(buf) == 0);
    assert(list_len(&buf->data.segments) == 0);

    buf_insert_char(buf, '0', 0);
    buf_insert_char(buf, '1', 1);
    buf_insert_char(buf, '2', 2);
    buf_insert_char(buf, '3', 3);
    buf_insert_char(buf, 'Q', 2);
    buf_remove_range(buf, buf_index_to_cursor(buf, 1),
                     buf_index_to_cursor(buf, 3));
    assert(buf_len(buf) == 2);
    assert(buf_content_eq(buf, "03"));
    assert(list_len(&buf->data.segments) == 2);

    return 0;
}
