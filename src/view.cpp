#include "view.hpp"


View
View::new_into_buffer(Buffer* b) {
    return View {
        b,
        0,
    };
}

Buffer::TmpCursor
View::cursor_at_start() const {
    return buffer->index_to_cursor(0);
}
