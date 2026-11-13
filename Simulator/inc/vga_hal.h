#ifndef VGA_HAL_H
#define VGA_HAL_H

#include <stdbool.h>
#include <stdint.h>

#define VGA_COLS 80
#define VGA_ROWS 25

#include "TUI/Event.h"

/* VRAM cell structure */
typedef struct
{
    unsigned char character;
    uint8_t attribute;
} VgaCell;

/* Compiler magic: Far pointer for 8086, normal pointer for modern PC */
#ifdef __ia16__
/* On 8086 we keep ONLY the 16-bit segment (e.g. 0xB800). This saves RAM and
 * cycles! */
extern uint16_t vga_segment;
#define MK_FP(seg, off)                                                        \
  ((VgaCell __far *)(((uint32_t)(seg) << 16) | (uint16_t)(off)))

/* Macro recreates the far pointer on the fly. GCC is smart enough
 * to generate a direct segment register access here (e.g. ES) */
#define VRAM_CELL(offset) (MK_FP(vga_segment, 0)[offset])
#else
/* On PC (Windows/SDL) we use a native, fast 32/64-bit pointer */
extern VgaCell* vga_buffer;
#define VRAM_CELL(offset) (vga_buffer[offset])
#endif

/* Buffer synchronization interface */
void vga_lock(void);
void vga_unlock(void);
/* Configuration macros for the TUI engine */
#define LOCK_BUFFER() vga_lock()
#define UNLOCK_BUFFER() vga_unlock()

/* HAL API */

bool vga_init(void);
bool vga_is_running(void);
bool vga_get_key(uint16_t* keycode);
void vga_draw_char(uint8_t x, uint8_t y, char ch, uint8_t attr);
void vga_draw_block(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t* buffer);
void vga_set_cursor(uint8_t x, uint8_t y);
void vga_shutdown(void);
void app_main(void);

#endif /* VGA_HAL_H */
