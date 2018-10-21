#include "list.hpp"


int
test_list_insert() {
    List<int> list = List<int>::new_empty();
    assert(list.len() == 0);

    List<int>::Node n1;
    n1.obj = 5;
    list.insert(&n1, null);
    assert(list.len() == 1);

    List<int>::Node n0;
    n0.obj = 2;
    list.insert(&n0, &n1);
    assert(list.len() == 2);

    List<int>::Node n2;
    n2.obj = 8;
    list.insert(&n2, null);
    assert(list.len() == 3);

    {
        const List<int>::Node* n = list.first;
        assert(n->obj == 2);
        n = n->next;
        assert(n->obj == 5);
        n = n->next;
        assert(n->obj == 8);
        n = n->next;
        assert(n == null);
    }
    {
        const List<int>::Node* n = list.last;
        assert(n->obj == 8);
        n = n->prev;
        assert(n->obj == 5);
        n = n->prev;
        assert(n->obj == 2);
        n = n->prev;
        assert(n == null);
    }

    return 0;
}

int
test_list_remove() {
    List<int> list = List<int>::new_empty();

    List<int>::Node n0;
    n0.obj = 2;
    list.add_last(&n0);

    List<int>::Node n1;
    n1.obj = 8;
    list.add_last(&n1);

    List<int>::Node n2;
    n2.obj = 5;
    list.add_last(&n2);

    List<int>::Node n3;
    n3.obj = 9;
    list.add_last(&n3);

    list.remove(&n2);
    assert(list.len() == 3);
    {
        const List<int>::Node*  n = list.first;
        assert(n->obj == 2);
        n = n->next;
        assert(n->obj == 8);
        n = n->next;
        assert(n->obj == 9);
        assert(n->next == null);
    }

    list.remove(&n0);
    assert(list.len() == 2);
    {
        const List<int>::Node* n = list.first;
        assert(n->obj == 8);
        n = n->next;
        assert(n->obj == 9);
        assert(n->next == null);
    }

    list.remove(&n3);
    assert(list.len() == 1);
    {
        const List<int>::Node* n = list.first;
        assert(n->obj == 8);
        assert(n->next == null);
    }

    list.remove(&n1);
    assert(list.len() == 0);
    {
        const List<int>::Node* n = list.first;
        assert(n == null);
    }

    return 0;
}
