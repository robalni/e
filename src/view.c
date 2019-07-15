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

    // The number of columns that fit in this view.
    u32 width;

    // The number of lines that fit in this view.
    u32 height;

    // The cursor that determines where text will be inserted.
    TmpCursor cursor;
};
typedef struct View View;
make_list_type(View);

struct ViewList {
    List(View) views;
    ListNode(View)* active_view;
};
typedef struct ViewList ViewList;

public void
new_view_into_buffer(ViewList* vl, Buffer* b) {
    ListNode(View)* node = mem_alloc(&b->mem, ListNode(View));
    list_add_last(vl, node);
    node->obj = (View) {
        .buffer = b,
        .offset_y = 0,
        .start_cursor = buf_cursor_at_start(b),
        .width = 80,
        .height = 5,
        .cursor = buf_cursor_at_start(b),
    };
    vl->active_view = node;
}

public View*
get_active_view(const ViewList* vl) {
    assert(vl);
    return &vl->active_view->obj;
}

public void
set_next_view_active(ViewList* vl) {
    assert(vl);
    vl->active_view = vl->active_view->next;
    if (vl->active_view == null) {
        vl->active_view = vl->views.first;
    }
}

public TmpCursor
view_cursor_at_start(const View* view) {
    assert(view);
    return view->start_cursor;
}
