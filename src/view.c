// A view into a buffer.  One buffer can have many views but one view
// can be into only one buffer.
struct View {
    Buffer* buffer;

    // How far into the buffer does this view begin?  If it's 48 that
    // means we have scrolled down 48 lines.
    u32 offset_y;

    // A cursor pointing to the first visible character.  It needs to
    // be renewed if the buffer is changed in a way that requires
    // that.
    TmpCursor start_cursor;

    // The number of lines that fit in this view.
    u32 height;

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
        .start_cursor = buf_cursor_at_start(b),
        .height = 5,
        .cursor = buf_cursor_at_start(b),
    };
}

public void
set_active_view(ViewList* vl, View* v) {
    assert(vl);
    assert(v);

    vl->active_view = v;
}

public TmpCursor
view_cursor_at_start(const View* view) {
    assert(view);
    // FIXME: This cursor will be invalid if the buffer is changed!
    return view->start_cursor;
}
