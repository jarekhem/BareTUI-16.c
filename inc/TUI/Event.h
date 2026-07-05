#pragma once

#include <stdint.h>

/* Key definitions - 16-bit DOS format: (scancode << 8) | ascii */
#define TUI_KEY_NONE  0x0000
#define TUI_KEY_ESC   0x011B
#define TUI_KEY_ENTER 0x1C0D
#define TUI_KEY_UP    0x4800
#define TUI_KEY_DOWN  0x5000
#define TUI_KEY_LEFT  0x4B00
#define TUI_KEY_RIGHT 0x4D00
#define TUI_KEY_SPACE 0x3920
#define TUI_KEY_TAB   0x0F09

typedef enum {
    TUI_EVENT_NONE,
    TUI_EVENT_KEY,
    TUI_EVENT_TICK,
    TUI_EVENT_FOCUS_GAIN,
    TUI_EVENT_FOCUS_LOST
} TUI_EventType;

typedef struct {
    TUI_EventType type;
    union {
        struct {
            uint16_t keycode;
        } key;
        struct {
            uint32_t delta_ms;
        } tick;
    };
} TUI_Event;