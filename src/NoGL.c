#include "NoGL.h"
#include "NoUtils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Surface surface_create(uint32_t width, uint32_t height) {
    Surface surface = {
            .width = width,
            .height = height,
            .pixels = malloc(width * height * sizeof(uint32_t))
    };
    return surface;
}

void surface_destroy(Surface surface) {
    free(surface.pixels);
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
        uint32_t pixel = surface.pixels[i];
        uint8_t bytes[3] = {
                pixel >> (8 * 0) & 0xFF,
                pixel >> (8 * 1) & 0xFF,
                pixel >> (8 * 2) & 0xFF,
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

void surface_fill(Surface surface, uint32_t color) {
    const uint32_t* end = &surface.pixels[surface.width * surface.height];
    for (uint32_t* cur = surface.pixels; cur != end; ++cur) {
        *cur = color;
    }
}

void draw_rect(Surface surface, int x, int y, int w, int h, uint32_t color) {
    for (int dy = 0; dy < h; ++dy) {
        int yi = y + dy;
        if (yi < 0 || yi >= surface.height) continue;
        for (int dx = 0; dx < w; ++dx) {
            int xi = x + dx;
            if (xi < 0 || xi >= surface.width) continue;
            surface.pixels[yi * surface.width + xi] = color;
        }
    }
}

void draw_circle(Surface surface, int x, int y, int r, uint32_t color) {
    int x2 = x + r;
    int y2 = y + r;
    for (int yi = y - r; yi < y2; ++yi) {
        if (yi < 0 || yi >= surface.height) continue;
        for (int xi = x - r; xi < x2; ++xi) {
            if (xi < 0 || xi >= surface.width) continue;
            int dx = xi - x;
            int dy = yi - y;
            if (dx * dx + dy * dy <= r * r) {
                surface.pixels[yi * surface.width + xi] = color;
            }
        }
    }
}

void draw_line(Surface surface, int x0, int y0, int x1, int y1, uint32_t color) {
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
                surface.pixels[y * surface.width + x] = color;
            }
        }
        else {  // slope greater than 1
            int orig_y1 = y1;
            if (y0 > y1) swap_ints(&y0, &y1);
            for (int y = y0; y < y1; ++y) {
                if (y < 0 || y >= surface.height) continue;
                int x = x1 + dx * (y - orig_y1) / dy;
                if (x < 0 || x >= surface.width) continue;
                surface.pixels[y * surface.width + x] = color;
            }
        }
    }
    else {  // vertical line
        int x = x1;
        if (x < 0 || x >= surface.width) return;
        if (y0 > y1) swap_ints(&y0, &y1);
        for (int y = y0; y < y1; ++y) {
            if (y < 0 || y >= surface.height) continue;
            surface.pixels[y * surface.width + x] = color;
        }
    }
}

Surface surface_copy(Surface original) {
    Surface copy = surface_create(original.width, original.height);
    memcpy(copy.pixels, original.pixels, copy.width * copy.height);
    return copy;
}

void surface_blit(Surface destination, Surface source, int x, int y) {
    for (int yi = 0; yi < source.height; ++yi) {
        if (yi + y < 0 || yi + y >= destination.height) continue;
        for (int xi = 0; xi < source.width; ++xi) {
            if (xi + x < 0 || xi + x >= destination.width) continue;
            destination.pixels[(y + yi) * destination.width + (x + xi)] = source.pixels[yi * source.width + xi];
        }
    }
}

Surface image_load(const char* filename) {
    int width, height, nrChannels;
    uint8_t* data = stbi_load(filename, &width, &height, &nrChannels, 4);

    Surface image = surface_create(width, height);
    memcpy(image.pixels, data, width * height * sizeof(uint32_t));

    stbi_image_free(data);
    return image;
}
