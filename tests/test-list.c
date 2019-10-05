make_list_type(int);

public int
test_list_insert() {
    List(int) list = {0};
    assert(list_len(&list) == 0);

    ListNode(int) n1;
    n1.obj = 5;
    list_insert(&list, &n1, null);
    assert(list_len(&list) == 1);

    ListNode(int) n0;
    n0.obj = 2;
    list_insert(&list, &n0, &n1);
    assert(list_len(&list) == 2);

    ListNode(int) n2;
    n2.obj = 8;
    list_insert(&list, &n2, null);
    assert(list_len(&list) == 3);

    {
        const ListNode(int)* n = list.first;
        assert(n->obj == 2);
        n = n->next;
        assert(n->obj == 5);
        n = n->next;
        assert(n->obj == 8);
        n = n->next;
        assert(n == null);
    }
    {
        const ListNode(int)* n = list.last;
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

public int
test_list_remove() {
    List(int) list = {0};

    ListNode(int) n0;
    n0.obj = 2;
    list_add_last(&list, &n0);

    ListNode(int) n1;
    n1.obj = 8;
    list_add_last(&list, &n1);

    ListNode(int) n2;
    n2.obj = 5;
    list_add_last(&list, &n2);

    ListNode(int) n3;
    n3.obj = 9;
    list_add_last(&list, &n3);

    list_remove(&list, &n2);
    assert(list_len(&list) == 3);
    {
        const ListNode(int)*  n = list.first;
        assert(n->obj == 2);
        n = n->next;
        assert(n->obj == 8);
        n = n->next;
        assert(n->obj == 9);
        assert(n->next == null);
    }

    list_remove(&list, &n0);
    assert(list_len(&list) == 2);
    {
        const ListNode(int)* n = list.first;
        assert(n->obj == 8);
        n = n->next;
        assert(n->obj == 9);
        assert(n->next == null);
    }

    list_remove(&list, &n3);
    assert(list_len(&list) == 1);
    {
        const ListNode(int)* n = list.first;
        assert(n->obj == 8);
        assert(n->next == null);
    }

    list_remove(&list, &n1);
    assert(list_len(&list) == 0);
    {
        const ListNode(int)* n = list.first;
        assert(n == null);
    }

    return 0;
}
