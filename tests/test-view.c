public int
test_view() {
    Buffer* buffer = new_buffer_empty(null, new_mem_default());
    ViewList vl = {0};
    new_view_into_buffer(&vl, buffer);

    return 0;
}
