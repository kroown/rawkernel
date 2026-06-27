#include "compositor.h"
#include <stddef.h>

compositor_t comp;

static int cursor_size = 16;

static uint8_t font8x8[256][8] = {
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x7e,0x81,0xa5,0x81,0xbd,0x99,0x81,0x7e },
    { 0x7e,0xff,0xdb,0xff,0xc3,0xe7,0xff,0x7e },
    { 0x6c,0xfe,0xfe,0xfe,0x7c,0x38,0x10,0x00 },
    { 0x10,0x38,0x7c,0xfe,0x7c,0x38,0x10,0x00 },
    { 0x38,0x7c,0x38,0xfe,0xfe,0xd6,0x10,0x38 },
    { 0x10,0x10,0x38,0x7c,0xfe,0x7c,0x10,0x38 },
    { 0x00,0x00,0x18,0x3c,0x3c,0x18,0x00,0x00 },
    { 0xff,0xff,0xe7,0xc3,0xc3,0xe7,0xff,0xff },
    { 0x00,0x3c,0x66,0x42,0x42,0x66,0x3c,0x00 },
    { 0xff,0xc3,0x99,0xbd,0xbd,0x99,0xc3,0xff },
    { 0x0f,0x07,0x0f,0x7d,0xcc,0xcc,0xcc,0x78 },
    { 0x3c,0x66,0x66,0x66,0x3c,0x18,0x7e,0x18 },
    { 0x3f,0x33,0x3f,0x30,0x30,0x70,0xf0,0xe0 },
    { 0x7f,0x63,0x7f,0x63,0x63,0x67,0xe6,0xc0 },
    { 0x18,0xdb,0x3c,0xe7,0xe7,0x3c,0xdb,0x18 },
    { 0x80,0xe0,0xf8,0xfe,0xf8,0xe0,0x80,0x00 },
    { 0x02,0x0e,0x3e,0xfe,0x3e,0x0e,0x02,0x00 },
    { 0x18,0x3c,0x7e,0x18,0x18,0x7e,0x3c,0x18 },
    { 0x66,0x66,0x66,0x66,0x66,0x00,0x66,0x00 },
    { 0x7f,0xdb,0xdb,0x7b,0x1b,0x1b,0x1b,0x00 },
    { 0x3e,0x63,0x38,0x6c,0x6c,0x38,0xcc,0x78 },
    { 0x00,0x00,0x00,0x00,0x7e,0x7e,0x7e,0x00 },
    { 0x18,0x3c,0x7e,0x18,0x7e,0x3c,0x18,0xff },
    { 0x18,0x3c,0x7e,0x18,0x18,0x18,0x18,0x00 },
    { 0x18,0x18,0x18,0x18,0x7e,0x3c,0x18,0x00 },
    { 0x00,0x18,0x0c,0xfe,0x0c,0x18,0x00,0x00 },
    { 0x00,0x30,0x60,0xfe,0x60,0x30,0x00,0x00 },
    { 0x00,0x00,0xc0,0xc0,0xc0,0xfe,0x00,0x00 },
    { 0x00,0x24,0x66,0xff,0x66,0x24,0x00,0x00 },
    { 0x00,0x18,0x3c,0x7e,0xff,0xff,0x00,0x00 },
    { 0x00,0xff,0xff,0x7e,0x3c,0x18,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x18,0x3c,0x3c,0x18,0x18,0x00,0x18,0x00 },
    { 0x6c,0x6c,0x6c,0x00,0x00,0x00,0x00,0x00 },
    { 0x6c,0x6c,0xfe,0x6c,0xfe,0x6c,0x6c,0x00 },
    { 0x18,0x3e,0x60,0x3c,0x06,0x7c,0x18,0x00 },
    { 0x00,0xc6,0xcc,0x18,0x30,0x66,0xc6,0x00 },
    { 0x38,0x6c,0x38,0x76,0xdc,0xcc,0x76,0x00 },
    { 0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00 },
    { 0x0c,0x18,0x30,0x30,0x30,0x18,0x0c,0x00 },
    { 0x30,0x18,0x0c,0x0c,0x0c,0x18,0x30,0x00 },
    { 0x00,0x66,0x3c,0xff,0x3c,0x66,0x00,0x00 },
    { 0x00,0x18,0x18,0x7e,0x18,0x18,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x18,0x18,0x30,0x00 },
    { 0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00 },
    { 0x06,0x0c,0x18,0x30,0x60,0xc0,0x80,0x00 },
    { 0x7c,0xc6,0xce,0xd6,0xe6,0xc6,0x7c,0x00 },
    { 0x18,0x38,0x18,0x18,0x18,0x18,0x7e,0x00 },
    { 0x7c,0xc6,0x06,0x1c,0x30,0x66,0xfe,0x00 },
    { 0x7c,0xc6,0x06,0x3c,0x06,0xc6,0x7c,0x00 },
    { 0x1c,0x3c,0x6c,0xcc,0xfe,0x0c,0x1e,0x00 },
    { 0xfe,0xc0,0xfc,0x06,0x06,0xc6,0x7c,0x00 },
    { 0x3c,0x60,0xc0,0xfc,0xc6,0xc6,0x7c,0x00 },
    { 0xfe,0xc6,0x0c,0x18,0x30,0x30,0x30,0x00 },
    { 0x7c,0xc6,0xc6,0x7c,0xc6,0xc6,0x7c,0x00 },
    { 0x7c,0xc6,0xc6,0x7e,0x06,0x0c,0x78,0x00 },
    { 0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00 },
    { 0x00,0x18,0x18,0x00,0x18,0x18,0x30,0x00 },
    { 0x0c,0x18,0x30,0x60,0x30,0x18,0x0c,0x00 },
    { 0x00,0x00,0x7e,0x00,0x7e,0x00,0x00,0x00 },
    { 0x30,0x18,0x0c,0x06,0x0c,0x18,0x30,0x00 },
    { 0x3c,0x66,0x0c,0x18,0x18,0x00,0x18,0x00 },
    { 0x7c,0xc6,0xde,0xde,0xdc,0xc0,0x7c,0x00 },
    { 0x18,0x3c,0x66,0x66,0xfe,0x66,0x66,0x00 },
    { 0xfc,0x66,0x66,0x7c,0x66,0x66,0xfc,0x00 },
    { 0x3c,0x66,0xc0,0xc0,0xc0,0x66,0x3c,0x00 },
    { 0xf8,0x6c,0x66,0x66,0x66,0x6c,0xf8,0x00 },
    { 0xfe,0x62,0x68,0x78,0x68,0x62,0xfe,0x00 },
    { 0xfe,0x62,0x68,0x78,0x68,0x60,0xf0,0x00 },
    { 0x3c,0x66,0xc0,0xc0,0xce,0x66,0x3e,0x00 },
    { 0x66,0x66,0x66,0x7e,0x66,0x66,0x66,0x00 },
    { 0x7e,0x18,0x18,0x18,0x18,0x18,0x7e,0x00 },
    { 0x1e,0x0c,0x0c,0x0c,0xcc,0xcc,0x78,0x00 },
    { 0xe6,0x66,0x6c,0x78,0x6c,0x66,0xe6,0x00 },
    { 0xf0,0x60,0x60,0x60,0x62,0x66,0xfe,0x00 },
    { 0xc6,0xee,0xfe,0xd6,0xc6,0xc6,0xc6,0x00 },
    { 0xc6,0xe6,0xf6,0xde,0xce,0xc6,0xc6,0x00 },
    { 0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,0x7c,0x00 },
    { 0xfc,0x66,0x66,0x7c,0x60,0x60,0xf0,0x00 },
    { 0x7c,0xc6,0xc6,0xc6,0xf6,0xde,0x7c,0x06 },
    { 0xfc,0x66,0x66,0x7c,0x6c,0x66,0xe6,0x00 },
    { 0x7c,0xc6,0x60,0x3c,0x06,0xc6,0x7c,0x00 },
    { 0xff,0x99,0x18,0x18,0x18,0x18,0x3c,0x00 },
    { 0x66,0x66,0x66,0x66,0x66,0x66,0x7c,0x00 },
    { 0x66,0x66,0x66,0x66,0x66,0x3c,0x18,0x00 },
    { 0xc6,0xc6,0xc6,0xd6,0xfe,0xee,0xc6,0x00 },
    { 0xc3,0x66,0x3c,0x18,0x3c,0x66,0xc3,0x00 },
    { 0xc3,0x66,0x3c,0x18,0x18,0x18,0x3c,0x00 },
    { 0xfe,0xcc,0x18,0x30,0x60,0xc6,0xfe,0x00 },
    { 0x3c,0x30,0x30,0x30,0x30,0x30,0x3c,0x00 },
    { 0xc0,0x60,0x30,0x18,0x0c,0x06,0x02,0x00 },
    { 0x3c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3c,0x00 },
    { 0x10,0x38,0x6c,0xc6,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff },
    { 0x30,0x18,0x0c,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x78,0x0c,0x7c,0xcc,0x76,0x00 },
    { 0xe0,0x60,0x7c,0x66,0x66,0x66,0xdc,0x00 },
    { 0x00,0x00,0x7c,0xc6,0xc0,0xc6,0x7c,0x00 },
    { 0x1c,0x0c,0x7c,0xcc,0xcc,0xcc,0x76,0x00 },
    { 0x00,0x00,0x7c,0xc6,0xfe,0xc0,0x7c,0x00 },
    { 0x3c,0x66,0x60,0xf8,0x60,0x60,0xf0,0x00 },
    { 0x00,0x00,0x76,0xcc,0xcc,0x7c,0x0c,0xf8 },
    { 0xe0,0x60,0x6c,0x76,0x66,0x66,0xe6,0x00 },
    { 0x18,0x00,0x38,0x18,0x18,0x18,0x3c,0x00 },
    { 0x06,0x00,0x06,0x06,0x06,0x66,0x66,0x3c },
    { 0xe0,0x60,0x66,0x6c,0x78,0x6c,0xe6,0x00 },
    { 0x38,0x18,0x18,0x18,0x18,0x18,0x3c,0x00 },
    { 0x00,0x00,0xec,0xfe,0xd6,0xc6,0xc6,0x00 },
    { 0x00,0x00,0xdc,0x66,0x66,0x66,0x66,0x00 },
    { 0x00,0x00,0x7c,0xc6,0xc6,0xc6,0x7c,0x00 },
    { 0x00,0x00,0xdc,0x66,0x66,0x7c,0x60,0xf0 },
    { 0x00,0x00,0x76,0xcc,0xcc,0x7c,0x0c,0x1e },
    { 0x00,0x00,0xdc,0x76,0x60,0x60,0xf0,0x00 },
    { 0x00,0x00,0x7e,0xc0,0x7c,0x06,0xfc,0x00 },
    { 0x30,0x30,0xfc,0x30,0x30,0x36,0x1c,0x00 },
    { 0x00,0x00,0xcc,0xcc,0xcc,0xcc,0x76,0x00 },
    { 0x00,0x00,0x66,0x66,0x66,0x3c,0x18,0x00 },
    { 0x00,0x00,0xc6,0xc6,0xd6,0xfe,0x6c,0x00 },
    { 0x00,0x00,0xc6,0x6c,0x38,0x6c,0xc6,0x00 },
    { 0x00,0x00,0xc6,0xc6,0xc6,0x7e,0x06,0xfc },
    { 0x00,0x00,0xfe,0x8c,0x18,0x62,0xfe,0x00 },
    { 0x0e,0x18,0x18,0x70,0x18,0x18,0x0e,0x00 },
    { 0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00 },
    { 0x70,0x18,0x18,0x0e,0x18,0x18,0x70,0x00 },
    { 0x76,0xdc,0x00,0x00,0x00,0x00,0x00,0x00 },
};

#define TERM_ROWS 25
#define TERM_COLS 80
#define TERM_LINES 256

static char term_lines[TERM_LINES][TERM_COLS];
static int term_line_count;
static int term_scroll;
static char term_input[TERM_COLS];
static int term_input_len;

static void term_putchar(char c) {
    if (c == '\b') {
        if (term_input_len > 0) term_input_len--;
        return;
    }
    if (c == '\n') {
        if (term_line_count < TERM_LINES) {
            int idx = term_line_count++;
            for (int i = 0; i < term_input_len && i < TERM_COLS - 1; i++)
                term_lines[idx][i] = term_input[i];
            term_lines[idx][term_input_len] = 0;
        }
        term_input_len = 0;
        if (term_line_count > TERM_ROWS)
            term_scroll = term_line_count - TERM_ROWS;
        return;
    }
    if (term_input_len < TERM_COLS - 1 && c >= 32) {
        term_input[term_input_len++] = c;
    }
}

static void term_write(const char *s) {
    for (; *s; s++) term_putchar(*s);
}

static int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

static int strncmp(const char *a, const char *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return (unsigned char)a[i] - (unsigned char)b[i];
        if (a[i] == 0) return 0;
    }
    return 0;
}

static void term_process_command(void) {
    if (term_input_len == 0) {
        term_putchar('\n');
        return;
    }
    char buf[TERM_COLS];
    int blen = term_input_len;
    for (int i = 0; i < blen && i < TERM_COLS - 1; i++)
        buf[i] = term_input[i];
    buf[blen] = 0;

    term_putchar('\n');

    if (strcmp(buf, "help") == 0) {
        term_write("help     show this help");
        term_putchar('\n');
        term_write("echo     repeat text");
        term_putchar('\n');
        term_write("clear    clear terminal");
        term_putchar('\n');
        term_write("uptime   show system uptime");
        term_putchar('\n');
        term_write("win      list open windows");
        term_putchar('\n');
        term_write("about    about Raw Kernel");
        term_putchar('\n');
    } else if (strcmp(buf, "clear") == 0) {
        term_line_count = 0;
        term_scroll = 0;
    } else if (strcmp(buf, "uptime") == 0) {
        term_write("uptime: running");
        term_putchar('\n');
    } else if (strcmp(buf, "win") == 0) {
        int n = 0;
        for (window_t *w = comp.windows; w; w = w->next)
            if (w->visible) n++;
        term_write("windows open: ");
        {
            char nbuf[16];
            int ni = 0, tmp = n;
            if (tmp == 0) nbuf[ni++] = '0';
            while (tmp > 0) { nbuf[ni++] = '0' + tmp % 10; tmp /= 10; }
            for (int i = 0; i < ni / 2; i++) { char t = nbuf[i]; nbuf[i] = nbuf[ni-1-i]; nbuf[ni-1-i] = t; }
            nbuf[ni] = 0;
            term_write(nbuf);
        }
        term_putchar('\n');
    } else if (strcmp(buf, "about") == 0) {
        term_write("Raw Kernel 0.1");
        term_putchar('\n');
        term_write("x86_64, BIOS, Limine");
        term_putchar('\n');
    } else if (strncmp(buf, "echo ", 5) == 0) {
        term_write(buf + 5);
        term_putchar('\n');
    } else if (buf[0] != 0) {
        term_write("unknown: ");
        term_write(buf);
        term_putchar('\n');
    }
    term_write("> ");
}

static void draw_rect(uint32_t *buf, int x, int y, int w, int h, uint32_t color) {
    int x1 = (x < 0) ? 0 : x;
    int y1 = (y < 0) ? 0 : y;
    int x2 = x + w; if ((uint64_t)x2 > comp.width) x2 = comp.width;
    int y2 = y + h; if ((uint64_t)y2 > comp.height) y2 = comp.height;
    for (int row = y1; row < y2; row++)
        for (int col = x1; col < x2; col++)
            buf[row * comp.width + col] = color;
}

static void draw_fill_rounded(uint32_t *buf, int x, int y, int w, int h, int r, uint32_t color) {
    int x1 = (x < 0) ? 0 : x;
    int y1 = (y < 0) ? 0 : y;
    int x2 = x + w; if ((uint64_t)x2 > comp.width) x2 = comp.width;
    int y2 = y + h; if ((uint64_t)y2 > comp.height) y2 = comp.height;
    for (int row = y1; row < y2; row++) {
        for (int col = x1; col < x2; col++) {
            int lx = col - x;
            int ly = row - y;
            int rx = x + w - 1 - col;
            int ry = y + h - 1 - row;
            if (lx < r && ly < r && lx * lx + ly * ly > r * r) continue;
            if (lx < r && ry < r && lx * lx + ry * ry > r * r) continue;
            if (rx < r && ly < r && rx * rx + ly * ly > r * r) continue;
            if (rx < r && ry < r && rx * rx + ry * ry > r * r) continue;
            buf[row * comp.width + col] = color;
        }
    }
}

static void draw_outline_rounded(uint32_t *buf, int x, int y, int w, int h, int r, uint32_t color) {
    for (int row = y; row < y + h; row++) {
        for (int col = x; col < x + w; col++) {
            int lx = col - x;
            int ly = row - y;
            int rx = x + w - 1 - col;
            int ry = y + h - 1 - row;
            bool inside;
            if (lx < r && ly < r) inside = (lx * lx + ly * ly <= r * r);
            else if (lx < r && ry < r) inside = (lx * lx + ry * ry <= r * r);
            else if (rx < r && ly < r) inside = (rx * rx + ly * ly <= r * r);
            else if (rx < r && ry < r) inside = (rx * rx + ry * ry <= r * r);
            else inside = true;
            if (!inside) continue;
            bool at_edge = (col == x || col == x + w - 1 || row == y || row == y + h - 1);
            if (!at_edge && lx < r && ly < r) {
                int nx = lx + 1, ny = ly;
                bool out = (nx * nx + ny * ny > r * r);
                nx = lx - 1; ny = ly; if (!out && lx > 0) out = (nx * nx + ny * ny > r * r);
                nx = lx; ny = ly + 1; if (!out) out = (nx * nx + ny * ny > r * r);
                nx = lx; ny = ly - 1; if (!out && ly > 0) out = (nx * nx + ny * ny > r * r);
                at_edge = out;
            }
            if (!at_edge && lx < r && ry < r) {
                int nx = lx + 1, ny = ry;
                bool out = (nx * nx + ny * ny > r * r);
                nx = lx - 1; ny = ry; if (!out && lx > 0) out = (nx * nx + ny * ny > r * r);
                nx = lx; ny = ry + 1; if (!out && ry > 0) out = (nx * nx + ny * ny > r * r);
                nx = lx; ny = ry - 1; if (!out) out = (nx * nx + ny * ny > r * r);
                at_edge = out;
            }
            if (!at_edge && rx < r && ly < r) {
                int nx = rx + 1, ny = ly;
                bool out = (nx * nx + ny * ny > r * r);
                nx = rx - 1; ny = ly; if (!out && rx > 0) out = (nx * nx + ny * ny > r * r);
                nx = rx; ny = ly + 1; if (!out) out = (nx * nx + ny * ny > r * r);
                nx = rx; ny = ly - 1; if (!out && ly > 0) out = (nx * nx + ny * ny > r * r);
                at_edge = out;
            }
            if (!at_edge && rx < r && ry < r) {
                int nx = rx + 1, ny = ry;
                bool out = (nx * nx + ny * ny > r * r);
                nx = rx - 1; ny = ry; if (!out && rx > 0) out = (nx * nx + ny * ny > r * r);
                nx = rx; ny = ry + 1; if (!out && ry > 0) out = (nx * nx + ny * ny > r * r);
                nx = rx; ny = ry - 1; if (!out) out = (nx * nx + ny * ny > r * r);
                at_edge = out;
            }
            if (at_edge)
                buf[row * comp.width + col] = color;
        }
    }
}

void comp_draw_char(void *buf, int x, int y, char c, uint32_t color) {
    uint32_t *fb = (uint32_t *)buf;
    for (int row = 0; row < 8; row++) {
        uint8_t glyph = font8x8[(unsigned char)c][row];
        for (int col = 0; col < 8; col++) {
            if (glyph & (1 << (7 - col))) {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && (uint64_t)px < comp.width && py >= 0 && (uint64_t)py < comp.height)
                    fb[py * comp.width + px] = color;
            }
        }
    }
}

void comp_draw_string(void *buf, int x, int y, const char *str, uint32_t color) {
    int cx = x;
    for (const char *p = str; *p; p++) {
        if (*p == '\n') { cx = x; y += 10; continue; }
        comp_draw_char(buf, cx, y, *p, color);
        cx += 9;
    }
}

void comp_draw_cursor(int x, int y) {
    uint32_t *back = comp.back;
    for (int row = 0; row < cursor_size; row++) {
        for (int col = 0; col < cursor_size; col++) {
            int px = x + col;
            int py = y + row;
            if (px < 0 || (uint64_t)px >= comp.width || py < 0 || (uint64_t)py >= comp.height)
                continue;
            bool fill = false;
            if (row < cursor_size - col - 1) fill = true;
            else if (row == 0 || col == 0 || row == cursor_size - col - 2) fill = true;
            if (fill) {
                uint32_t cur_col = (col < cursor_size / 3) ? 0x000000 : 0xFFFFFF;
                if (col == 0 || row == 0 || col == cursor_size - 1 || row == cursor_size - 1)
                    cur_col = 0x000000;
                back[py * comp.width + px] = cur_col;
            }
        }
    }
}

static uint32_t *fb_addr;

void comp_init(uint64_t fb, uint64_t width, uint64_t height, uint64_t pitch) {
    comp.addr = fb;
    comp.width = width;
    comp.height = height;
    comp.pitch = pitch;
    comp.windows = NULL;
    comp.active = NULL;
    comp.mouse_x = width / 2;
    comp.mouse_y = height / 2;
    comp.mouse_buttons = 0;
    comp.prev_mouse_buttons = 0;
    comp.start_menu_open = false;
    comp.cursor_dirty = true;

    fb_addr = (uint32_t *)(uintptr_t)fb;

    static uint32_t pool[1920 * 1080];
    if (width * height <= 1920 * 1080) {
        comp.back = pool;
    } else {
        comp.back = fb_addr;
    }

    for (size_t i = 0; i < (size_t)width * height; i++)
        comp.back[i] = 0;

    term_line_count = 0;
    term_scroll = 0;
    term_input_len = 0;
}

window_t *window_create(const char *title, int x, int y, int w, int h) {
    window_t *win = (window_t *)0x60000;
    static int alloc_off = 0;
    static char pool[32768];
    if (alloc_off + sizeof(window_t) <= (int)sizeof(pool)) {
        win = (window_t *)(pool + alloc_off);
        alloc_off += sizeof(window_t);
    }

    for (int i = 0; title[i] && i < 63; i++) win->title[i] = title[i];
    win->title[63] = 0;
    win->x = x; win->y = y;
    win->width = w; win->height = h;
    win->visible = true;
    win->minimised = false;
    win->dragging = false;
    win->dirty = true;
    win->type = WINDOW_NORMAL;

    size_t buf_size = (size_t)w * h;
    static char buf_pool[262144];
    static int buf_off = 0;
    if (buf_off + (int)(buf_size * 4) <= (int)sizeof(buf_pool)) {
        win->buf = (uint32_t *)(buf_pool + buf_off);
        buf_off += buf_size * 4;
    } else {
        win->buf = NULL;
    }

    if (win->buf) {
        for (size_t i = 0; i < buf_size; i++)
            win->buf[i] = 0x1E1E2E;
    }

    win->next = comp.windows;
    comp.windows = win;
    comp.cursor_dirty = true;
    return win;
}

void window_destroy(window_t *win) {
    if (comp.active == win) comp.active = NULL;
    window_t **pp = &comp.windows;
    while (*pp) {
        if (*pp == win) { *pp = win->next; break; }
        pp = &(*pp)->next;
    }
    comp.cursor_dirty = true;
}

window_t *window_at(int x, int y) {
    window_t *result = NULL;
    for (window_t *w = comp.windows; w; w = w->next) {
        if (!w->visible || w->minimised) continue;
        if (x >= w->x && x < w->x + w->width && y >= w->y && y < w->y + w->height)
            result = w;
    }
    return result;
}

void window_raise(window_t *win) {
    if (!win || comp.windows == win) return;
    window_t **pp = &comp.windows;
    while (*pp) {
        if (*pp == win) { *pp = win->next; break; }
        pp = &(*pp)->next;
    }
    win->next = comp.windows;
    comp.windows = win;
    comp.cursor_dirty = true;
}

void window_close_click(window_t *win) {
    if (win) {
        win->visible = false;
        if (comp.active == win) comp.active = NULL;
    }
}

static void render_terminal_text(window_t *win) {
    if (!win->buf) return;
    int ww = win->width, wh = win->height;
    uint32_t bg = 0x0F0F0F;
    uint32_t fg = 0xA6E3A1;
    int pad_x = 8, pad_y = 6;

    for (int row = TITLEBAR_H; row < wh; row++) {
        for (int col = 0; col < ww; col++) {
            win->buf[row * ww + col] = bg;
        }
    }

    int visible_rows = (wh - TITLEBAR_H) / 10;
    if (visible_rows < 1) visible_rows = 1;
    if (visible_rows > TERM_ROWS) visible_rows = TERM_ROWS;

    int start_line = term_line_count - visible_rows;
    if (start_line < 0) start_line = 0;

    for (int r = 0; r < visible_rows && start_line + r < term_line_count; r++) {
        int yp = TITLEBAR_H + r * 10;
        int li = start_line + r;
        for (int c = 0; c < TERM_COLS && term_lines[li][c]; c++) {
            comp_draw_char(win->buf, pad_x + c * 9, yp + pad_y, term_lines[li][c], fg);
        }
    }

    int input_row = (term_line_count - start_line) * 10 + TITLEBAR_H;
    if (input_row < wh) {
        int ci = 0;
        for (int c = 0; c < term_input_len && c < TERM_COLS; c++) {
            comp_draw_char(win->buf, pad_x + c * 9, input_row + pad_y, term_input[c], fg);
            ci = c + 1;
        }
        int blink = (term_line_count + term_input_len) & 4;
        if (blink && ci * 9 + pad_x < ww)
            draw_rect(win->buf, pad_x + ci * 9, input_row + pad_y, 6, 8, fg);
    }
}

static void render_start_menu(void) {
    if (!comp.start_menu_open) return;
    uint32_t *back = comp.back;
    int sx = 0;
    int sy = comp.height - TASKBAR_H - 320;
    int sw = 220;
    int sh = 320;

    draw_rect(back, sx, sy, sw, sh, 0x11111B);
    draw_outline_rounded(back, sx, sy, sw, sh, 3, 0x45475A);

    const char *items[] = { "Terminal", "About" };
    int n_items = 2;
    for (int i = 0; i < n_items; i++) {
        int iy = sy + 8 + i * 36;
        int iw = sw - 16;
        int ix = sx + 8;
        draw_rect(back, ix, iy, iw, 32, 0x1E1E2E);
        draw_rect(back, ix, iy, iw, 32, 0x313244);
        comp_draw_string(back, ix + 10, iy + 10, items[i], 0xCDD6F4);
    }
}

static void render_window(window_t *win) {
    if (!win->visible || win->minimised) return;

    if (win->type == WINDOW_TERMINAL)
        render_terminal_text(win);

    uint32_t *back = comp.back;
    int wx = win->x, wy = win->y, ww = win->width, wh = win->height;

    draw_rect(back, wx + 5, wy + 5, ww, wh, 0x06060E);

    uint32_t tb_col = (win == comp.active) ? 0x363950 : 0x262734;
    uint32_t ac_col = (win == comp.active) ? 0x89B4FA : 0x45475A;
    uint32_t close_bg = 0xF38BA8;
    uint32_t close_x = 0x1E1E2E;
    int r = 4;

    for (int row = 0; row < wh; row++) {
        for (int col = 0; col < ww; col++) {
            int px = wx + col, py = wy + row;
            if (px < 0 || (uint64_t)px >= comp.width || py < 0 || (uint64_t)py >= comp.height)
                continue;

            uint32_t color;
            if (row < TITLEBAR_H) {
                int lx = col, ly = row;
                int rx = ww - 1 - col;
                if (lx < r && ly < r && lx * lx + ly * ly > r * r) { color = 0; }
                else if (rx < r && ly < r && rx * rx + ly * ly > r * r) { color = 0; }
                else if (row < 2) {
                    color = ac_col;
                } else if (col >= ww - 28 && col < ww - 4 && row >= 6 && row < 22) {
                    int cx = col - (ww - 28);
                    if (cx == 0 || cx == 23 || row == 6 || row == 21) { color = 0; }
                    else if (cx >= 2 && cx < 22 && row >= 8 && row < 20) {
                        color = close_bg;
                    } else { color = tb_col; }
                } else {
                    color = tb_col;
                }
            } else {
                if (win->buf) {
                    uint32_t bc = win->buf[row * ww + col];
                    color = (bc == 0) ? 0x1E1E2E : bc;
                } else {
                    color = 0x1E1E2E;
                }
                if (row == TITLEBAR_H) color = 0x585B70;
            }
            back[py * comp.width + px] = color;
        }
    }

    int cx = wx + ww - 16;
    int cy = wy + 10;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            int xx = cx + i * 3 + j;
            int yy = cy + i * 3 + j;
            if (xx >= wx + ww - 28 && xx < wx + ww - 4 && yy >= wy + 8 && yy < wy + 20)
                draw_rect(back, xx, yy, 1, 1, close_x);
            yy = cy + (8 - i * 3) + j;
            if (xx >= wx + ww - 28 && xx < wx + ww - 4 && yy >= wy + 8 && yy < wy + 20)
                draw_rect(back, xx, yy, 1, 1, close_x);
        }
    }

    int tx = wx + 10;
    int ty = wy + (TITLEBAR_H - 8) / 2;
    for (const char *p = win->title; *p; p++) {
        comp_draw_char(back, tx, ty, *p, 0xCDD6F4);
        tx += 9;
    }

    draw_outline_rounded(back, wx, wy, ww, wh, r, 0x585B70);
}

static void render_desktop(void) {
    uint32_t *back = comp.back;
    for (uint64_t y = 0; y < comp.height - TASKBAR_H; y++) {
        uint32_t h = (uint32_t)(comp.height - TASKBAR_H);
        uint8_t t = (uint8_t)((y * 30) / h);
        uint32_t color = (t << 16) | ((t/2) << 8) | (0x2E + t);
        for (uint64_t x = 0; x < comp.width; x++)
            back[y * comp.width + x] = color;
    }
}

static void render_taskbar(void) {
    uint32_t *back = comp.back;
    int tb_y = comp.height - TASKBAR_H;

    draw_rect(back, 0, tb_y, comp.width, TASKBAR_H, 0x0F0F17);

    for (int x = 0; x < (int)comp.width; x++)
        back[tb_y * comp.width + x] = 0x89B4FA;

    draw_rect(back, 0, tb_y, START_BUTTON_W, TASKBAR_H, 0x89B4FA);
    comp_draw_string(back, 14, tb_y + (TASKBAR_H - 8) / 2, "Start", 0x0F0F17);

    int bx = START_BUTTON_W + 4;
    int max_task_w = ((int)comp.width - START_BUTTON_W - 200) / 2;
    if (max_task_w < 80) max_task_w = 80;
    for (window_t *w = comp.windows; w && bx + max_task_w < (int)comp.width - 100; w = w->next) {
        if (!w->visible) continue;
        bool act = (w == comp.active);
        draw_rect(back, bx, tb_y + 3, max_task_w, TASKBAR_H - 6, act ? 0x2E3045 : 0x1A1B28);
        if (act)
            draw_rect(back, bx, tb_y + 3, max_task_w, 2, 0x89B4FA);
        int tl = 0;
        for (int i = 0; w->title[i] && i < 16; i++) tl += 9;
        if (tl > max_task_w - 12) tl = max_task_w - 12;
        int tx = bx + (max_task_w - tl) / 2;
        int ty = tb_y + (TASKBAR_H - 8) / 2;
        for (const char *p = w->title; *p && tx + 9 < bx + max_task_w - 4; p++) {
            comp_draw_char(back, tx, ty, *p, act ? 0xCDD6F4 : 0x6C7086);
            tx += 9;
        }
        bx += max_task_w + 4;
    }

    const char *label = "Raw Kernel";
    int lw = 0;
    for (const char *p = label; *p; p++) lw += 9;
    int lx = ((int)comp.width - lw - 10);
    int ly = tb_y + (TASKBAR_H - 8) / 2;
    for (const char *p = label; *p; p++) {
        comp_draw_char(back, lx, ly, *p, 0x585B70);
        lx += 9;
    }
}

void comp_render(void) {
    render_desktop();

    window_t *windows[64];
    int nw = 0;
    for (window_t *w = comp.windows; w && nw < 64; w = w->next) {
        if (w->visible && !w->minimised)
            windows[nw++] = w;
    }
    for (int i = nw - 1; i >= 0; i--)
        render_window(windows[i]);

    render_taskbar();
    render_start_menu();
    comp_draw_cursor(comp.mouse_x, comp.mouse_y);
}

void comp_swap(void) {
    uint64_t count = comp.width * comp.height;
    uint32_t *dst = fb_addr;
    uint32_t *src = comp.back;
    __asm__ volatile("rep movsd" : "+D"(dst), "+S"(src), "+c"(count) : : "memory");
}

void comp_handle_click(void) {
    int mx = comp.mouse_x;
    int my = comp.mouse_y;
    int tb_y = comp.height - TASKBAR_H;

    if (my >= tb_y && mx < START_BUTTON_W) {
        comp.start_menu_open = !comp.start_menu_open;
        return;
    }

    int bx = START_BUTTON_W + 4;
    int max_task_w = ((int)comp.width - START_BUTTON_W - 200) / 2;
    if (max_task_w < 80) max_task_w = 80;
    if (my >= tb_y) {
        for (window_t *w = comp.windows; w && bx + max_task_w < (int)comp.width - 100; w = w->next) {
            if (!w->visible) continue;
            if (mx >= bx && mx < bx + max_task_w && my >= tb_y + 3 && my < tb_y + TASKBAR_H - 3) {
                window_raise(w);
                comp.active = w;
                return;
            }
            bx += max_task_w + 4;
        }
        comp.start_menu_open = false;
    }

    if (comp.start_menu_open) {
        int sx = 0;
        int sy = tb_y - 320;
        int sw = 220;
        int sh = 320;
        if (mx >= sx && mx < sx + sw && my >= sy && my < sy + sh) {
            int item_idx = (my - sy - 8) / 36;
            if (item_idx >= 0 && item_idx < 2) {
                comp.start_menu_open = false;
                if (item_idx == 0) {
                    window_t *existing = NULL;
                    for (window_t *w = comp.windows; w; w = w->next) {
                        if (w->type == WINDOW_TERMINAL && w->visible) {
                            existing = w;
                            break;
                        }
                    }
                    if (existing) {
                        window_raise(existing);
                        comp.active = existing;
                    } else {
                        window_t *term = window_create("Terminal", 60, 40, 748, 28 + 25*10 + 2);
                        if (term) {
                            term->type = WINDOW_TERMINAL;
                            comp.active = term;
                            term_write("Raw Kernel Terminal v0.1");
                            term_putchar('\n');
                            term_write("type 'help' for commands");
                            term_putchar('\n');
                            term_write("> ");
                        }
                    }
                } else if (item_idx == 1) {
                    window_t *abt = window_create("About Raw Kernel", 120, 100, 400, 200);
                    if (abt && abt->buf) {
                        comp.active = abt;
                        comp_draw_string(abt->buf, 20, 40, "Raw Kernel 0.1", 0xA6E3A1);
                        comp_draw_string(abt->buf, 20, 56, "x86_64, BIOS, Limine", 0xA6ADC8);
                        comp_draw_string(abt->buf, 20, 72, "Simple GUI compositor", 0xA6ADC8);
                        comp_draw_string(abt->buf, 20, 88, "2016-2026", 0xA6ADC8);
                    }
                }
                return;
            }
        }
        comp.start_menu_open = false;
    }

    for (window_t *w = comp.windows; w; w = w->next) {
        if (!w->visible || w->minimised) continue;
        int cl_x = w->x + w->width - 28;
        int cl_y = w->y + 6;
        if (mx >= cl_x && mx < cl_x + 24 && my >= cl_y && my < cl_y + 14) {
            window_close_click(w);
            return;
        }
    }

    window_t *hit = window_at(mx, my);
    if (hit) {
        window_raise(hit);
        comp.active = hit;
        if (my >= hit->y && my < hit->y + TITLEBAR_H) {
            hit->dragging = true;
            hit->drag_off_x = mx - hit->x;
            hit->drag_off_y = my - hit->y;
        }
    } else {
        comp.active = NULL;
    }
}

void comp_handle_key(int ch) {
    if (comp.active && comp.active->type == WINDOW_TERMINAL) {
        if (ch == '\b') {
            term_putchar('\b');
        } else if (ch == '\n' || ch == '\r') {
            term_process_command();
        } else {
            term_putchar(ch);
        }
    }
}
