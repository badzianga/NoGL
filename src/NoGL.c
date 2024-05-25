#include "NoGL.h"
#include "NoUtils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <immintrin.h>

// Color module ----------------------------------------------------------------------------------------------------- //
Color color_from_hex(uint32_t value) {
    return *(Color*)&value;
}

uint32_t color_to_hex(Color color) {
    return *(uint32_t*)&color;
}

Color color_grayscale(Color color) {
    uint8_t value = (uint32_t)color.r * 2126 / 10000 + (uint32_t)color.g * 7152 / 10000 + (uint32_t)color.b * 722 / 10000;
    color.r = value;
    color.g = value;
    color.b = value;
    return color;
}

Color color_premul_alpha(Color color) {
    color.r = color.r * color.a / 255;
    color.g = color.g * color.a / 255;
    color.b = color.b * color.a / 255;
    return color;
}


// Surface module --------------------------------------------------------------------------------------------------- //
Surface surface_create(uint32_t width, uint32_t height) {
    Surface surface = {
            .width = width,
            .height = height,
            .pixels = malloc(width * height * sizeof(Color))
    };
    return surface;
}

void surface_destroy(Surface surface) {
    free(surface.pixels);
}

void surface_fill(Surface surface, Color color) {
//    const Color* end = &surface.pixels[surface.width * surface.height];
//    for (Color* cur = surface.pixels; cur != end; ++cur) {
//        *cur = color;
//    }

//    __m128i color_simd = _mm_set1_epi32(*(int*)&color);
//    uint32_t block_count = surface.width * surface.height / 4;
//    __m128i* end = (__m128i*)&surface.pixels[block_count * 4];
//    for (__m128i* cur = (__m128i*)surface.pixels; cur != end; ++cur) {
//        _mm_storeu_si128(cur, color_simd);
//    }
//    for (uint32_t i = block_count * 4; i < surface.width * surface.height; ++i) {
//        surface.pixels[i] = color;
//    }

    __m256i color_simd = _mm256_set1_epi32(*(int*)&color);
    uint32_t block_count = surface.width * surface.height / 8;
    __m256i* end = (__m256i*)&surface.pixels[block_count * 8];
    for (__m256i* cur = (__m256i*)surface.pixels; cur != end; ++cur) {
        _mm256_storeu_si256(cur, color_simd);
    }
    for (uint32_t i = block_count * 8; i < surface.width * surface.height; ++i) {
        surface.pixels[i] = color;
    }
}

void surface_blit(Surface destination, Surface source, int x, int y) {
    for (int yi = 0; yi < source.height; ++yi) {
        if (yi + y < 0 || yi + y >= destination.height) continue;
        for (int xi = 0; xi < source.width; ++xi) {
            if (xi + x < 0 || xi + x >= destination.width) continue;
#ifdef ALPHA_BLENDING
            Color prev_color = destination.pixels[(y + yi) * destination.width + (x + xi)];
            destination.pixels[(y + yi) * destination.width + (x + xi)] = mix_colors(prev_color, source.pixels[yi * source.width + xi]);
#else
            destination.pixels[(y + yi) * destination.width + (x + xi)] = source.pixels[yi * source.width + xi];
#endif
        }
    }
}

Surface surface_copy(Surface original) {
    Surface copy = surface_create(original.width, original.height);
    memcpy(copy.pixels, original.pixels, copy.width * copy.height);
    return copy;
}


// Drawing module --------------------------------------------------------------------------------------------------- //
void draw_rect(Surface surface, int x, int y, int w, int h, Color color) {
    for (int dy = 0; dy < h; ++dy) {
        int yi = y + dy;
        if (yi < 0 || yi >= surface.height) continue;
        for (int dx = 0; dx < w; ++dx) {
            int xi = x + dx;
            if (xi < 0 || xi >= surface.width) continue;
#ifdef ALPHA_BLENDING
            Color prev_color = surface.pixels[yi * surface.width + xi];
            surface.pixels[yi * surface.width + xi] = mix_colors(prev_color, color);
#else
            surface.pixels[yi * surface.width + xi] = color;
#endif
        }
    }
}

void draw_circle(Surface surface, int x, int y, int r, Color color) {
    int x2 = x + r;
    int y2 = y + r;
    for (int yi = y - r; yi < y2; ++yi) {
        if (yi < 0 || yi >= surface.height) continue;
        for (int xi = x - r; xi < x2; ++xi) {
            if (xi < 0 || xi >= surface.width) continue;
            int dx = xi - x;
            int dy = yi - y;
            if (dx * dx + dy * dy <= r * r) {
#ifdef ALPHA_BLENDING
                Color prev_color = surface.pixels[yi * surface.width + xi];
                surface.pixels[yi * surface.width + xi] = mix_colors(prev_color, color);
#else
                surface.pixels[yi * surface.width + xi] = color;
#endif
            }
        }
    }
}

void draw_line(Surface surface, int x0, int y0, int x1, int y1, Color color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    if (dx != 0) {  // non-vertical line
        if (abs(dx) >= abs(dy)) {  // slope less or equal to 1
            int orig_x1 = x1;
            if (x0 > x1) swap_ints(&x0, &x1);
            for (int x = x0; x < x1; ++x) {
                if (x < 0 || x >= surface.width) continue;
                int y = y1 + dy * (x - orig_x1) / dx;
                if (y < 0 || y >= surface.height) continue;
#ifdef ALPHA_BLENDING
                Color prev_color = surface.pixels[y * surface.width + x];
                surface.pixels[y * surface.width + x] = mix_colors(prev_color, color);
#else
                surface.pixels[y * surface.width + x] = color;
#endif
            }
        }
        else {  // slope greater than 1
            int orig_y1 = y1;
            if (y0 > y1) swap_ints(&y0, &y1);
            for (int y = y0; y < y1; ++y) {
                if (y < 0 || y >= surface.height) continue;
                int x = x1 + dx * (y - orig_y1) / dy;
                if (x < 0 || x >= surface.width) continue;
#ifdef ALPHA_BLENDING
                Color prev_color = surface.pixels[y * surface.width + x];
                surface.pixels[y * surface.width + x] = mix_colors(prev_color, color);
#else
                surface.pixels[y * surface.width + x] = color;
#endif
            }
        }
    }
    else {  // vertical line
        int x = x1;
        if (x < 0 || x >= surface.width) return;
        if (y0 > y1) swap_ints(&y0, &y1);
        for (int y = y0; y < y1; ++y) {
            if (y < 0 || y >= surface.height) continue;
#ifdef ALPHA_BLENDING
            Color prev_color = surface.pixels[y * surface.width + x];
            surface.pixels[y * surface.width + x] = mix_colors(prev_color, color);
#else
            surface.pixels[y * surface.width + x] = color;
#endif
        }
    }
}


// Image module ----------------------------------------------------------------------------------------------------- //
Surface image_load_ppm(const char* file_path) {
    FILE* f = fopen(file_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "[ERROR] Failed to open file: %s\n", file_path);
        // TODO: created surfaces aren't deleted
        exit(1);
    }
    char buffer[32];
    if (!fgets(buffer, sizeof(buffer), f)) {
        perror(file_path);
        exit(1);
    }
    if (buffer[0] != 'P' || buffer[1] != '6') {
        fprintf(stderr, "[ERROR] Invalid image format (must be 'P6')\n");
        exit(1);
    }

    //check for comments
    int c = getc(f);
    while (c == '#') {
        while (getc(f) != '\n') ;
        c = getc(f);
    }
    ungetc(c, f);

    uint32_t w, h;
    if (fscanf(f, "%d %d", &w, &h) != 2) {
        fprintf(stderr, "[ERROR] Invalid image size\n");
        exit(1);
    }
    printf("Image size: %dx%d\n", w, h);

    Surface image = surface_create(w, h);
    while (fgetc(f) != '\n');
    fgetc(f);

    for (uint32_t i = 0; i < w * h; ++i) {
        Color pixel;
        pixel.a = 255;
        fscanf(f, "%c %c %c", &pixel.b, &pixel.g, &pixel.r);
        image.pixels[i] = pixel;
    }
//    if (fread(image.pixels, 3 * image.width, image.height, f) != image.height) {
//        fprintf(stderr, "Error loading image \n");
//        surface_destroy(image);
//        exit(1);
//    }

    fclose(f);
    return image;
}

void image_save_ppm(Surface surface, const char* file_path) {
    FILE* f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "[ERROR] Failed to open file: %s\n", file_path);
        surface_destroy(surface);
        exit(1);
    }

    fprintf(f, "P6\n%u %u\n255\n", surface.width, surface.height);
    if (ferror(f)) {
        fprintf(stderr, "[ERROR] Failed to write header to file: %s\n", file_path);
        fclose(f);
        surface_destroy(surface);
        exit(1);
    }

    for (size_t i = 0; i < surface.width * surface.height; ++i) {
        Color pixel = surface.pixels[i];
        uint8_t bytes[3] = {
                pixel.r,
                pixel.g,
                pixel.b,
        };
        fwrite(bytes, sizeof(bytes), 1, f);
        if (ferror(f)) {
            fprintf(stderr, "[ERROR] Failed to write data to file: %s\n", file_path);
            fclose(f);
            surface_destroy(surface);
            exit(1);
        }
    }
    fclose(f);
}

// Font module ------------------------------------------------------------------------------------------------------ //
#define CHARACTERS 26
#define CHAR_WIDTH 3
#define CHAR_HEIGHT 5
const char DEFAULT_FONT[CHARACTERS][CHAR_HEIGHT][CHAR_WIDTH] = {
        {  // A
                { 0, 1, 0 },
                { 1, 0, 1 },
                { 1, 1, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
        },
        {  // B
                { 1, 1, 0 },
                { 1, 0, 1 },
                { 1, 1, 0 },
                { 1, 0, 1 },
                { 1, 1, 0 },
        },
        {  // C
                { 0, 1, 0 },
                { 1, 0, 1 },
                { 1, 0, 0 },
                { 1, 0, 1 },
                { 0, 1, 0 },
        },
        {  // D
                { 1, 1, 0 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 1, 0 },
        },
        {  // E
                { 1, 1, 1 },
                { 1, 0, 0 },
                { 1, 1, 0 },
                { 1, 0, 0 },
                { 1, 1, 1 },
        },
        {  // F
                { 1, 1, 1 },
                { 1, 0, 0 },
                { 1, 1, 0 },
                { 1, 0, 0 },
                { 1, 0, 0 },
        },
        {  // G
                { 0, 1, 1 },
                { 1, 0, 0 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 0, 1, 1 },
        },
        {  // H
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 1, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
        },
        {  // I
                { 0, 1, 0 },
                { 0, 1, 0 },
                { 0, 1, 0 },
                { 0, 1, 0 },
                { 0, 1, 0 },
        },
        {  // J
                { 0, 0, 1 },
                { 0, 0, 1 },
                { 0, 0, 1 },
                { 1, 0, 1 },
                { 0, 1, 0 },
        },
        {  // K
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 1, 0 },
                { 1, 0, 1 },
                { 1, 0, 1 },
        },
        {  // L
                { 1, 0, 0 },
                { 1, 0, 0 },
                { 1, 0, 0 },
                { 1, 0, 0 },
                { 1, 1, 1 },
        },
        {  // M
                { 1, 0, 1 },
                { 1, 1, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
        },
        {  // N
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 1, 1 },
                { 1, 1, 1 },
                { 1, 0, 1 },
        },
        {  // O
                { 0, 1, 0 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 0, 1, 0 },
        },
        {  // P
                { 1, 1, 0 },
                { 1, 0, 1 },
                { 1, 1, 0 },
                { 1, 0, 0 },
                { 1, 0, 0 },
        },
        {  // Q
                { 0, 1, 0 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 1, 1 },
                { 0, 1, 1 },
        },
        {  // R
                { 1, 1, 0 },
                { 1, 0, 1 },
                { 1, 1, 0 },
                { 1, 0, 1 },
                { 1, 0, 1 },
        },
        {  // S
                { 0, 1, 1 },
                { 1, 0, 0 },
                { 0, 1, 0 },
                { 0, 0, 1 },
                { 1, 1, 0 },
        },
        {  // T
                { 1, 1, 1 },
                { 0, 1, 0 },
                { 0, 1, 0 },
                { 0, 1, 0 },
                { 0, 1, 0 },
        },
        {  // U
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 0, 1, 0 },
        },
        {  // V
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 0, 1, 0 },
                { 0, 1, 0 },
        },
        {  // W
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 1, 1, 1 },
                { 1, 0, 1 },
        },
        {  // X
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 0, 1, 0 },
                { 1, 0, 1 },
                { 1, 0, 1 },
        },
        {  // Y
                { 1, 0, 1 },
                { 1, 0, 1 },
                { 0, 1, 0 },
                { 0, 1, 0 },
                { 0, 1, 0 },
        },
        {  // Z
                { 1, 1, 1 },
                { 0, 0, 1 },
                { 0, 1, 0 },
                { 1, 0, 0 },
                { 1, 1, 1 },
        },
        
};

void draw_char(Surface surface, int x, int y, char character, Color color) {
    character -= 65;
    if (character < 0 || character >= 26) return;
    for (int yi = 0; yi < CHAR_HEIGHT; ++yi) {
        int dy = y + yi;
        if (dy < 0 || dy >= surface.height) continue;
        for (int xi = 0; xi < CHAR_WIDTH; ++xi) {
            int dx = x + xi;
            if (dx < 0 || dx >= surface.width) continue;
            if (DEFAULT_FONT[character][yi][xi]) {
                surface.pixels[dy * surface.width + dx] = color;
            }
        }
    }
}

void draw_text(Surface surface, int x, int y, const char* text, Color color) {
    int len = (int)strlen(text);
    for (int i = 0; i < len; ++i) {
        draw_char(surface, x + i * (CHAR_WIDTH + 1), y, text[i], color);
    }
}

// Display module --------------------------------------------------------------------------------------------------- //
static Surface display_surface = { 0 };
static Display* display;
static Window window;
static XWindowAttributes wa = { 0 };
static XImage* image;
static GC gc;
static Atom wm_delete_window;
static int should_close = 0;

Surface display_init(int width, int height, const char* title) {
    display_surface = surface_create(width, height);
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "[ERROR] Failed to connect to X server!\n");
    }
    window = XCreateSimpleWindow(display, XDefaultRootWindow(display), 0, 0, width, height, 0, 0, 0);
//    XSetWindowAttributes xwa = {.background_pixel = WhitePixel(display, XDefaultRootWindow(display)), .border_pixel = BlackPixel(display, XDefaultRootWindow(display))};
    XStoreName(display, window, title);

    XGetWindowAttributes(display, window, &wa);
    image = XCreateImage(display, wa.visual, wa.depth, ZPixmap, 0, (char*)display_surface.pixels, display_surface.width, display_surface.height, 32, 0);
    image->red_mask = 0x000000FF;
    image->green_mask = 0x0000FF00;
    image->blue_mask = 0x00FF0000;
    image->byte_order = LSBFirst;
    image->bitmap_unit = 32;
    image->bitmap_bit_order = LSBFirst;
    gc = XCreateGC(display, window, 0, NULL);
    XSelectInput(display, window, KeyPressMask);  // | ExposureMask
    XMapWindow(display, window);
    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);
    XSync(display, False);
    
    return display_surface;
}

void display_quit() {
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    surface_destroy(display_surface);
}

int display_should_close() {
    return should_close;
}

void display_update() {
    while (XPending(display) > 0) {
        XEvent event = { 0 };
        XNextEvent(display, &event);
        switch (event.type) {
            case KeyPress:
                printf("Key pressed!\n");
                break;
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
                    should_close = 1;
                }
                break;
        }
    }
    XPutImage(display, window, gc, image, 0, 0, 0, 0, display_surface.width, display_surface.height);
    XSync(display, False);
}
