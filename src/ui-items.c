struct Minibuf {
    char* data;
    usize len;
    usize allocd;
};
typedef struct Minibuf Minibuf;

public void
minibuf_insert_char(Minibuf* buf, char c, u32 where, Memory* mem) {
    if (buf->len >= buf->allocd) {
        if (buf->allocd < 32) {
            buf->allocd = 32;
        } else {
            buf->allocd *= 2;
        }
        char* new_mem = mem_alloc_size(mem, buf->allocd);

        usize new_i = 0;
        usize buf_i = 0;
        while (buf_i < buf->len && new_i < buf->allocd) {
            new_mem[new_i] = buf->data[buf_i];
            buf_i++;
            new_i++;
        }
        if (new_i < buf->allocd) {
            new_mem[new_i] = c;
            new_i++;
        }
        while (buf_i < buf->len && new_i < buf->allocd) {
            new_mem[new_i] = buf->data[buf_i];
        }

        buf->data = new_mem;
    } else {
        usize i = buf->len;
        while (i > where) {
            buf->data[i] = buf->data[i - 1];
            i--;
        }
        buf->data[i] = c;
    }
    buf->len++;
}

////////////
// Textbox

struct Textbox {
    Minibuf buf;
    u32 cursor;
};
typedef struct Textbox Textbox;

enum TextboxEventType {
    TEXTBOX_NONE,
    TEXTBOX_SUBMIT,
};

struct TextboxEvent {
    enum TextboxEventType type;
};
typedef struct TextboxEvent TextboxEvent;

public TextboxEvent
update_textbox(Textbox* tb, const Event* ev, Memory* mem) {
    if (ev->type & EVENT_KEYDOWN) {
        if (ev->keysym == '\n') {
            return (TextboxEvent) {
                .type = TEXTBOX_SUBMIT,
            };
        }
    }
    if (ev->type & EVENT_CHAR) {
        minibuf_insert_char(&tb->buf, ev->ch, tb->cursor, mem);
        tb->cursor++;
    }
    return (TextboxEvent) {
        .type = TEXTBOX_NONE,
    };
}

public void
minibuf_get_content(const Minibuf* buf, char* to, usize to_len) {
    usize i;
    for (i = 0; i < buf->len && i < to_len - 1; i++) {
        to[i] = buf->data[i];
    }
    to[i] = '\0';
}
