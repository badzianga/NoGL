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

// Surface module --------------------------------------------------------------------------------------------------- //
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t* pixels;
} Surface;

Surface surface_create(uint32_t width, uint32_t height);
void surface_destroy(Surface surface);
void surface_blit(Surface destination, Surface source, int x, int y);
Surface surface_copy(Surface original);
void surface_fill(Surface surface, uint32_t color);

// Drawing module --------------------------------------------------------------------------------------------------- //
// TODO: implement alpha blending
void draw_rect(Surface surface, int x, int y, int w, int h, uint32_t color);
void draw_circle(Surface surface, int x, int y, int r, uint32_t color);
void draw_line(Surface surface, int x0, int y0, int x1, int y1, uint32_t color);
void draw_ellipse(Surface surface, ...);  // TODO: not implemented
void draw_triangle(Surface surface, ...);  // TODO: not implemented
void draw_aacircle(Surface surface, int x, int y, int r, uint32_t color);  // TODO: not implemented
void draw_aaline(Surface surface, int x0, int y0, int x1, int y1, uint32_t color);  // TODO: not implemented
void draw_aaellipse(Surface surface, ...);  // TODO: not implemented
void draw_aatriangle(Surface surface, int x0, int y0, int x1, int y1, uint32_t color);  // TODO: not implemented

// Image module
Surface image_load(const char* filename);  // TODO: remove or re-implement
void image_save_ppm(Surface surface, const char* file_path);
void image_save_jpg(Surface surface, const char* file_path);  // TODO: not implemented
void image_save_png(Surface surface, const char* file_path);  // TODO: not implemented
void image_save_bmp(Surface surface, const char* file_path);  // TODO: not implemented

// display module
// TODO:
//  Display_init will create a window and rendering surface.
//  The surface will be returned, but the copy will exist in display module.
Surface display_init(int width, int height, const char* title);  // TODO: not implemented
void display_quit();  // TODO: not implemented
void display_update();  // TODO: not implemented

#endif //NOGL_H
