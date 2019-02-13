#include "view.hpp"


int
test_view() {
    Buffer buffer = Buffer::new_empty(MemoryManager::new_default());
    View view = View::new_into_buffer(&buffer);

    return 0;
}
