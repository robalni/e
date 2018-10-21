#include "common.hpp"
#include "list.hpp"


template <typename T>
List<T>
List<T>::new_empty() {
    return List {
        null,
        null,
    };
}

template <typename T>
void
List<T>::add_last(Node* node) {
    node->prev = this->last;
    node->next = null;
    if (node->prev) {
        node->prev->next = node;
    }
    this->last = node;
    if (this->first == null) {
        this->first = node;
    }
}

template <typename T>
void
List<T>::insert(Node* node, Node* before_node) {
    if (before_node == null) {
        return this->add_last(node);
    }
    node->prev = before_node->prev;
    node->next = before_node;
    if (node->prev) {
        node->prev->next = node;
    } else {
        this->first = node;
    }
    before_node->prev = node;
}

template <typename T>
void
List<T>::remove(const Node* node) {
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        this->first = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        this->last = node->prev;
    }
}

template <typename T>
usize
List<T>::len() const {
    Node* node = this->first;
    usize count = 0;
    while (node) {
        count++;
        node = node->next;
    }
    return count;
}

template <typename T>
typename List<T>::Node*
List<T>::obj_to_node(T* obj) {
    return reinterpret_cast<Node*>(obj);
}
