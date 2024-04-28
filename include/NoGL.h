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
} Surface;

Surface surface_create(uint32_t width, uint32_t height);
void surface_destroy(Surface surface);
void surface_save_to_ppm(Surface surface, const char* file_path);
void surface_fill(Surface surface, uint32_t color);
void draw_rect(Surface surface, int x, int y, int w, int h, uint32_t color);
void draw_circle(Surface surface, int x, int y, int r, uint32_t color);
void draw_line(Surface surface, int x0, int y0, int x1, int y1, uint32_t color);

#endif //NOGL_H
