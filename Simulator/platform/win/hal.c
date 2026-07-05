#include <assert.h>
#include <SDL.h>
#include <stdbool.h>
#include "../../Simulator/inc/vga_hal.h"

#include <stdio.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

static VgaCell local_vram[VGA_COLS * VGA_ROWS];
VgaCell* vga_buffer = local_vram;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* font_texture = NULL;
static volatile bool is_running = true;

SDL_mutex* vram_mutex = NULL;
static SDL_Thread* render_thread = NULL;

static int RenderThreadTask(void* ptr);

/* Standard VGA 16-color palette */
static const uint32_t vga_palette[16] = {
    0x000000, 0x0000AA, 0x00AA00, 0x00AAAA,
    0xAA0000, 0xAA00AA, 0xAA5500, 0xAAAAAA,
    0x555555, 0x5555FF, 0x55FF55, 0x55FFFF,
    0xFF5555, 0xFF55FF, 0xFFFF55, 0xFFFFFF
};

bool vga_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    vram_mutex = SDL_CreateMutex();
    /* Start the background rendering thread */
    render_thread = SDL_CreateThread(RenderThreadTask, "VgaRenderThread", NULL);
    if (!render_thread)
        return false;

    /* Run until the emulator signals a shutdown */
    window = SDL_CreateWindow("VGA Text Mode Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, VGA_COLS * FONT_WIDTH, VGA_ROWS * FONT_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
        return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    char* base_path = SDL_GetBasePath();
    char bmp_path[1024];
    snprintf(bmp_path, sizeof(bmp_path), "%svga-rom-fonts-ag868x16.bmp", base_path ? base_path : "");
    if (base_path) SDL_free(base_path);
    SDL_Surface* font_surface = SDL_LoadBMP(bmp_path);
    if (font_surface)
    {
        SDL_SetColorKey(font_surface, SDL_TRUE, SDL_MapRGB(font_surface->format, 0, 0, 0));
        font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
        SDL_SetTextureBlendMode(font_texture, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(font_surface);
    }
    else
    {
        printf("Warning: Failed to load font BMP: %s\n", SDL_GetError());
    }

    return true;
}


void vga_lock(void)
{
    SDL_LockMutex(vram_mutex);
}

void vga_unlock(void)
{
    SDL_UnlockMutex(vram_mutex);
}

bool vga_is_running(void)
{
    return is_running;
}

void vga_draw_char(uint8_t x, uint8_t y, unsigned char ch, uint8_t attr)
{
    assert(x < 80 && y < 25);

    uint16_t offset = (uint16_t)(y * 80) + x;
    vga_lock();
    vga_buffer[offset].character = ch;
    vga_buffer[offset].attribute = attr;
    vga_unlock();
}

/* Map SDL keys to our engine keys */
static uint16_t translate_key(SDL_Keycode sym)
{
    switch (sym)
    {
    case SDLK_UP: return TUI_KEY_UP;
    case SDLK_DOWN: return TUI_KEY_DOWN;
    case SDLK_LEFT: return TUI_KEY_LEFT;
    case SDLK_RIGHT: return TUI_KEY_RIGHT;
    case SDLK_ESCAPE: return TUI_KEY_ESC;
    case SDLK_RETURN: return TUI_KEY_ENTER;
    case SDLK_TAB: return TUI_KEY_TAB;
    case SDLK_SPACE: return TUI_KEY_SPACE;
    default:
        /* Return standard ASCII if printable */
        if (sym >= 32 && sym <= 126) return (uint16_t)sym;
        return TUI_KEY_NONE;
    }
}

void vga_draw_block(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint16_t* buffer)
{
    for (uint8_t row = 0; row < h; row++)
    {
        for (uint8_t col = 0; col < w; col++)
        {
            uint16_t cell = buffer[(row * w) + col];
            unsigned char ch = (unsigned char)(cell & 0xFF);
            uint8_t attr = (uint8_t)(cell >> 8);
            vga_draw_char(x + col, y + row, ch, attr);
        }
    }
}

bool vga_get_key(uint16_t* keycode)
{
    SDL_Event event;
    /* Poll event queue until we find a keypress or empty the queue */
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            is_running = false;
            *keycode = TUI_KEY_ESC;
            return true;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            *keycode = translate_key(event.key.keysym.sym);
            return true;
        }
    }
    return false;
}

void vga_update(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < VGA_ROWS; y++)
    {
        for (int x = 0; x < VGA_COLS; x++)
        {
            VgaCell cell = vga_buffer[y * VGA_COLS + x];
            uint8_t fg_idx = cell.attribute & 0x0F;
            uint8_t bg_idx = (cell.attribute >> 4) & 0x0F;

            SDL_Rect dest_rect = {x * FONT_WIDTH, y * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT};

            /* Draw Background */
            uint32_t bg_color = vga_palette[bg_idx];
            SDL_SetRenderDrawColor(renderer, (bg_color >> 16) & 0xFF, (bg_color >> 8) & 0xFF, bg_color & 0xFF, 255);
            SDL_RenderFillRect(renderer, &dest_rect);

            /* Draw Foreground Character */
            if (font_texture)
            {
                uint8_t u_char = (uint8_t)cell.character;
                SDL_Rect src_rect = {(u_char % 16) * FONT_WIDTH, (u_char / 16) * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT};
                uint32_t fg_color = vga_palette[fg_idx];
                SDL_SetTextureColorMod(font_texture, (fg_color >> 16) & 0xFF, (fg_color >> 8) & 0xFF, fg_color & 0xFF);
                SDL_RenderCopy(renderer, font_texture, &src_rect, &dest_rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(32);
}

void vga_set_cursor(uint8_t x, uint8_t y)
{
    (void)x;
    (void)y;
}

void vga_shutdown(void)
{
    is_running = false;

    /* Wait for the thread to finish executing before destroying resources */
    if (render_thread)
    {
        SDL_WaitThread(render_thread, NULL);
    }

    if (vram_mutex) SDL_DestroyMutex(vram_mutex);
    if (font_texture) SDL_DestroyTexture(font_texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

/* ------------------------------------------------------------------
 * 1. THE THREAD FUNCTION
 * This runs in the background, continuously flushing VRAM to the screen.
 * ------------------------------------------------------------------ */
static int RenderThreadTask(void* ptr)
{
    (void)ptr;
    is_running = true;
    while (is_running)
    {
        /* Safely lock the buffer before reading local_vram */
        vga_lock();

        vga_update();
        /* Unlock immediately so the TUI engine can continue writing */
        vga_unlock();
        /* Sleep for ~16ms to maintain 60 FPS and prevent 100% CPU usage */
        SDL_Delay(16);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    printf("Starting emulator...\n");
    app_main();
    printf("Emulator finished\n");
    return 0;
}
