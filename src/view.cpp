#include "view.hpp"


View
View::new_into_buffer(Buffer* b) {
    return View {
        b,
        0,
    };
}
