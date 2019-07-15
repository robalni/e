#define List(T) struct List_ ## T

#define ListNode(T) struct ListNode_ ## T

#define make_list_type(T)            \
    struct List_ ## T {              \
        ListNode(T)* first;          \
        ListNode(T)* last;           \
    };                               \
    struct ListNode_ ## T {          \
        ListNode(T)* prev;           \
        ListNode(T)* next;           \
        T obj;                       \
    }

struct ListNodeDummy {
    struct ListNodeDummy* prev;
    struct ListNodeDummy* next;
};

struct ListDummy {
    struct ListNodeDummy* first;
    struct ListNodeDummy* last;
};

#define new_list_empty(T)            \
    (List(T)) {                      \
        .first = null,               \
        .last = null,                \
    }

// LIST_VOID must be of type List(T)
// NODE_VOID must be of type ListNode(T)
public void
list_add_last(void* list_void, void* node_void) {
    struct ListDummy* list = list_void;
    assert(list);
    struct ListNodeDummy* node = node_void;
    assert(node);

    node->prev = list->last;
    node->next = null;
    if (node->prev) {
        node->prev->next = node;
    }
    list->last = node;
    if (list->first == null) {
        list->first = node;
    }
}

// Inserts NODE before BEFORE_NODE or last if BEFORE_NODE is null.
// LIST_VOID must be of type List(T)
// NODE_VOID must be of type ListNode(T)
// BEFORE_NODE_VOID must be of type ListNode(T)
public void
list_insert(void* list_void, void* node_void, void* before_node_void) {
    struct ListDummy* list = list_void;
    assert(list);
    struct ListNodeDummy* node = node_void;
    assert(node);
    struct ListNodeDummy* before_node = before_node_void;

    if (before_node == null) {
        return list_add_last(list, node);
    }
    node->prev = before_node->prev;
    node->next = before_node;
    if (node->prev) {
        node->prev->next = node;
    } else {
        list->first = node;
    }
    before_node->prev = node;
}

// LIST_VOID must be of type List(T)
// NODE_VOID must be of type ListNode(T)
public void
list_remove(void* list_void, void* node_void) {
    struct ListDummy* list = list_void;
    assert(list);
    struct ListNodeDummy* node = node_void;
    assert(node);

    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->first = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->last = node->prev;
    }
}

// LIST_VOID must be of type List(T)
public usize
list_len(const void* list_void) {
    const struct ListDummy* list = list_void;
    assert(list);

    const struct ListNodeDummy* node = list->first;
    usize count = 0;
    while (node) {
        count++;
        node = node->next;
    }
    return count;
}

public void*
list_obj_to_node(void* obj) {
    return obj - sizeof (void*) * 2;
}
