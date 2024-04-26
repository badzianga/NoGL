#ifndef NOGL_H
#define NOGL_H
#include <stdint.h>

#define RED     0xFF0000FF
#define GREEN   0xFF00FF00
#define BLUE    0xFFFF0000
#define BLACK   0xFF000000
#define WHITE   0xFFFFFFFF
#define YELLOW  0xFF00FFFF
#define MAGENTA 0xFFFF00FF
#define CYAN    0xFFFFFF00

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t* pixels;
} Canvas;

Canvas canvas_create(uint32_t width, uint32_t height);
void canvas_destroy(Canvas canvas);
void canvas_save_to_ppm(Canvas canvas, const char* file_path);
void canvas_fill_iter(Canvas canvas, uint32_t color);
void canvas_fill_ptr(Canvas canvas, uint32_t color);
void canvas_fill_avx2(Canvas canvas, uint32_t color);
void canvas_fill_sse(Canvas canvas, uint32_t color);

#endif //NOGL_H
