#ifndef NOGL_H
#define NOGL_H
#include <stdint.h>

// Color module ----------------------------------------------------------------------------------------------------- //
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

#define RED     (Color){ 255, 0, 0, 255 }
#define GREEN   (Color){ 0, 255, 0, 255 }
#define BLUE    (Color){ 0, 0, 255, 255 }
#define BLACK   (Color){ 0, 0, 0, 255 }
#define WHITE   (Color){ 255, 255, 255, 255 }
#define YELLOW  (Color){ 255, 255, 0, 255 }
#define MAGENTA (Color){ 255, 0, 255, 255 }
#define CYAN    (Color){ 0, 255, 255, 255 }

// Surface module --------------------------------------------------------------------------------------------------- //
typedef struct {
    uint32_t width;
    uint32_t height;
    Color* pixels;
} Surface;

Surface surface_create(uint32_t width, uint32_t height);
void surface_destroy(Surface surface);
void surface_fill(Surface surface, Color color);
void surface_blit(Surface destination, Surface source, int x, int y);
Surface surface_copy(Surface original);

// Drawing module --------------------------------------------------------------------------------------------------- //
void draw_rect(Surface surface, int x, int y, int w, int h, Color color);
void draw_circle(Surface surface, int x, int y, int r, Color color);
void draw_line(Surface surface, int x0, int y0, int x1, int y1, Color color);

// Image module ----------------------------------------------------------------------------------------------------- //
Surface image_load_ppm(const char* file_path);
void image_save_ppm(Surface surface, const char* file_path);

// Display module --------------------------------------------------------------------------------------------------- //
// TODO:
//  Display_init will create a window and rendering surface.
//  The surface will be returned, but the copy will exist in display module.
Surface display_init(int width, int height, const char* title);  // TODO: not implemented
void display_quit();  // TODO: not implemented
void display_update();  // TODO: not implemented

// Font module ------------------------------------------------------------------------------------------------------ //
void draw_text(Surface surface, int x, int y, const char* text, Color color);

#endif //NOGL_H
