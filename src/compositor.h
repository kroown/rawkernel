#ifndef _COMPOSITOR_H
#define _COMPOSITOR_H

#include <stdint.h>
#include <stdbool.h>

#define TITLEBAR_H 28
#define TASKBAR_H 36
#define START_BUTTON_W 100

#define WINDOW_NORMAL 0
#define WINDOW_TERMINAL 1

typedef struct window {
    int x, y;
    int width, height;
    char title[64];
    bool visible;
    bool minimised;
    bool dragging;
    int drag_off_x, drag_off_y;

    uint32_t *buf;
    bool dirty;

    int type;

    struct window *next;
} window_t;

typedef struct {
    uint64_t addr;
    uint64_t width, height, pitch;
    uint32_t *back;
    window_t *windows;
    window_t *active;
    int mouse_x, mouse_y;
    uint8_t mouse_buttons;
    uint8_t prev_mouse_buttons;
    bool start_menu_open;
    bool cursor_dirty;
} compositor_t;

extern compositor_t comp;

void comp_init(uint64_t fb_addr, uint64_t width, uint64_t height, uint64_t pitch);

window_t *window_create(const char *title, int x, int y, int w, int h);
void window_destroy(window_t *win);
void window_close_click(window_t *win);
window_t *window_at(int x, int y);
void window_raise(window_t *win);

void comp_render(void);
void comp_swap(void);
void comp_handle_click(void);
void comp_handle_key(int ch);

void comp_draw_char(void *buf, int x, int y, char c, uint32_t color);
void comp_draw_string(void *buf, int x, int y, const char *str, uint32_t color);
void comp_draw_cursor(int x, int y);

#endif
