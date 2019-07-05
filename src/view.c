// A view into a buffer.  One buffer can have many views but one view
// can be into only one buffer.
struct View {
    Buffer* buffer;

    // How far into the buffer does this view begin?  If it's 48 that
    // means we have scrolled down 48 lines.
    u32 offset_y;

    // The cursor that determines where text will be inserted.
    TmpCursor cursor;
};
typedef struct View View;
make_list_type(View);

struct ViewList {
    List(View) views;
    View* active_view;
};
typedef struct ViewList ViewList;

public View
new_view_into_buffer(Buffer* b) {
    return (View) {
        .buffer = b,
        .offset_y = 0,
        .cursor = buf_cursor_at_start(b),
    };
}

// Returns cursor pointing to the first character in this view.
public TmpCursor
cursor_at_start(const View* view) {
    assert(view);
    return buf_index_to_cursor(view->buffer, 0);
}

public void
set_active_view(ViewList* vl, const View* v) {
    assert(vl);
    assert(v);

    vl->active_view = v;
}
