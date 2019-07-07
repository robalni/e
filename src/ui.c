// This is UI code that is common for all operating systems.  More
// specific UI code for different operating systems can be found in
// the os directory.

enum Key {
    KEY_UNKNOWN,
    KEY_UP,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_DOWN,
    KEY_HOME,
    KEY_END,
    KEY_RETURN,
    KEY_BACKSPACE,
    KEY_DEL,
    KEY_ESCAPE,
    KEY_PAGEDOWN,
    KEY_PAGEUP,
};
typedef enum Key Key;

enum EventType {
    EVENT_UNKNOWN = 0,
    EVENT_CHAR = 1,
    EVENT_KEYDOWN = 2,
    EVENT_QUIT = 4,
    EVENT_RENDER = 8,
};
struct Event {
    int type;
    int keysym;
};
typedef struct Event Event;

public bool
key_is_printable(int keysym) {
    return (keysym >= 32 && keysym < 127) || keysym == '\n';
}
