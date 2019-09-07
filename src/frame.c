enum FrameType {
    FRAME_EDIT,
    FRAME_SAVE,
};

struct Frame {
    enum FrameType type;
    union {
        struct EditFrame {  // type == FRAME_EDIT
            ViewList vl;
            BufferList bl;
        } edit;
        struct SaveFrame {  // type == FRAME_SAVE
            const Buffer* buf;
            struct EditFrame* edit_frame;
            Textbox textbox;
        } save;
    };
};
typedef struct Frame Frame;
make_list_type(Frame);

struct FrameList {
    List(Frame) frames;
    ListNode(Frame) bg_frame;
    ListNode(Frame) fg_frame;
};
typedef struct FrameList FrameList;

public void
set_bg_frame(FrameList* wl, Frame w) {
    wl->bg_frame.obj = w;
    list_remove(wl, wl->frames.first);
    list_add_last(wl, &wl->bg_frame);
}

public void
set_fg_frame(FrameList* wl, Frame w) {
    wl->fg_frame.obj = w;
    if (list_len(wl) > 1) {
        list_remove(wl, wl->frames.last);
    }
    list_add_last(wl, &wl->fg_frame);
}

public void
close_fg_frame(FrameList* wl) {
    list_remove(wl, wl->frames.last);
}

public Frame*
get_active_frame(FrameList* wl) {
    return wl->frames.first != wl->frames.last
        ? &wl->fg_frame.obj
        : &wl->bg_frame.obj;
}
