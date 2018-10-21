#pragma once


template <typename T>
struct List {
    struct Node {
        T obj;
        Node* prev;
        Node* next;
    };

    static List
    new_empty();

    void
    add_last(Node* node);

    // Inserts NODE before BEFORE_NODE or last if BEFORE_NODE is null.
    void
    insert(Node* node, Node* before_node);

    void
    remove(const Node* node);

    usize
    len() const;

    static Node*
    obj_to_node(T* obj);


    // Internal

    Node* first;
    Node* last;

    List() = delete;
};
