#include "NoGL.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <immintrin.h>

Canvas canvas_create(uint32_t width, uint32_t height) {
    Canvas canvas = {
            .width = width,
            .height = height,
            .pixels = malloc(width * height * sizeof(uint32_t))  // normal
            //.pixels = aligned_alloc(32, width * height * sizeof(uint32_t))  // avx2
            //.pixels = aligned_alloc(16, width * height * sizeof(uint32_t))  // sse
    };
    return canvas;
}

void canvas_destroy(Canvas canvas) {
    free(canvas.pixels);
}

void canvas_save_to_ppm(Canvas canvas, const char* file_path) {
    FILE* f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "[ERROR] Failed to open file: %s\n", file_path);
        canvas_destroy(canvas);
        exit(1);
    }

    fprintf(f, "P6\n%u %u\n255\n", canvas.width, canvas.height);
    if (ferror(f)) {
        fprintf(stderr, "[ERROR] Failed to write header to file: %s\n", file_path);
        fclose(f);
        canvas_destroy(canvas);
        exit(1);
    }

    for (size_t i = 0; i < canvas.width * canvas.height; ++i) {
        uint32_t pixel = canvas.pixels[i];
        uint8_t bytes[3] = {
                pixel >> (8 * 0) & 0xFF,
                pixel >> (8 * 1) & 0xFF,
                pixel >> (8 * 2) & 0xFF,
        };
        fwrite(bytes, sizeof(bytes), 1, f);
        if (ferror(f)) {
            fprintf(stderr, "[ERROR] Failed to write data to file: %s\n", file_path);
            fclose(f);
            canvas_destroy(canvas);
            exit(1);
        }
    }
    fclose(f);
}

void canvas_fill_iter(Canvas canvas, uint32_t color) {
    const uint32_t iterations = canvas.width * canvas.height;
    for (uint32_t i = 0; i < iterations; ++i) {
        canvas.pixels[i] = color;
    }
}

void canvas_fill_ptr(Canvas canvas, uint32_t color) {
    const uint32_t* end = &canvas.pixels[canvas.width * canvas.height];
    for (uint32_t* cur = canvas.pixels; cur != end; ++cur) {
        *cur = color;
    }
}

void canvas_fill_avx2(Canvas canvas, uint32_t color) {
    __m256i color_simd = _mm256_set1_epi32(*(int*)&color);
    uint32_t block_count = canvas.width * canvas.height / 8;
    __m256i* end = (__m256i*)&canvas.pixels[block_count * 8];
    for (__m256i* cur = (__m256i*)canvas.pixels; cur != end; ++cur) {
        _mm256_storeu_si256(cur, color_simd);
    }
    for (uint32_t i = block_count * 8; i < canvas.width * canvas.height; ++i) {
        canvas.pixels[i] = color;
    }
}

void canvas_fill_sse(Canvas canvas, uint32_t color) {
    __m128i color_simd = _mm_set1_epi32(*(int*)&color);
    uint32_t block_count = canvas.width * canvas.height / 4;
    __m128i* end = (__m128i*)&canvas.pixels[block_count * 4];
    for (__m128i* cur = (__m128i*)canvas.pixels; cur != end; ++cur) {
        _mm_storeu_si128(cur, color_simd);
    }
    for (uint32_t i = block_count * 4; i < canvas.width * canvas.height; ++i) {
        canvas.pixels[i] = color;
    }
}

void rect_fill(Canvas canvas, int x, int y, int w, int h, uint32_t color) {
    for (int dy = 0; dy < h; ++dy) {
        int yi = y + dy;
        if (yi < 0 || yi >= canvas.height) continue;
        for (int dx = 0; dx < w; ++dx) {
            int xi = x + dx;
            if (xi < 0 || xi >= canvas.width) continue;
            canvas.pixels[yi * canvas.width + xi] = color;
        }
    }
}
