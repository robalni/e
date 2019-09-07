struct Textbox {
    Buffer buffer;
};
typedef struct Textbox Textbox;

enum TextboxEventType {
    TEXTBOX_SUBMIT,
};

struct TextboxEvent {
    enum TextboxEventType type;
};
typedef struct TextboxEvent TextboxEvent;

public TextboxEvent
update_textbox(const Event* ev) {
}
