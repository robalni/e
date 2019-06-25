#pragma once


struct Buffer;

// A view into a buffer.  One buffer can have many views but one view
// can be into only one buffer.
struct View {
    static View
    new_into_buffer(Buffer* b);

    // Returns cursor pointing to the first character in this view.
    Buffer::TmpCursor
    cursor_at_start() const;

    Buffer* buffer;

    // How far into the buffer does this view begin?  If it's 48 that
    // means we have scrolled down 48 lines.
    u32 offset_y;

    // The cursor that determines where text will be inserted.
    Buffer::TmpCursor cursor;

    View() = delete;
};
