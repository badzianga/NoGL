#include "NoUtils.h"

void swap_ints(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

Color mix_colors(Color c1, Color c2) {
    c1.r = (uint16_t)c1.r + (uint16_t)(c2.r - c1.r) * (uint16_t)c2.a / 255;  // TODO: or c1.a?
    c1.g = (uint16_t)c1.g + (uint16_t)(c2.g - c1.g) * (uint16_t)c2.a / 255;
    c1.b = (uint16_t)c1.b + (uint16_t)(c2.b - c1.b) * (uint16_t)c2.a / 255;
    c1.a = (uint16_t)c1.a + (uint16_t)(c2.a - c1.a) * (uint16_t)c2.a / 255;
    return c1;
}
