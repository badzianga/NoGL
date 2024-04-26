#include "NoGL.h"
#include <time.h>
#include <stdio.h>

int main(void) {
    clock_t start;
    clock_t end;

    Canvas canvas = canvas_create(800, 600);  // 33618 - magic number oooh boiii
    printf("Canvas size:  %ux%u\n", canvas.width, canvas.height);

    start = clock();
    canvas_fill_iter(canvas, YELLOW);
    end = clock();
    printf("Index time:   %.3fms\n", (float)(end - start) * 1000.f / CLOCKS_PER_SEC);
    canvas_save_to_ppm(canvas, "iteration.ppm");

    start = clock();
    canvas_fill_ptr(canvas, YELLOW);
    end = clock();
    printf("Pointer time: %.3fms\n", (float)(end - start) * 1000.f / CLOCKS_PER_SEC);
    canvas_save_to_ppm(canvas, "pointer.ppm");

    start = clock();
    canvas_fill_avx2(canvas, YELLOW);
    end = clock();
    printf("AVX2 time:    %.3fms\n", (float)(end - start) * 1000.f / CLOCKS_PER_SEC);
    canvas_save_to_ppm(canvas, "avx2.ppm");

    start = clock();
    canvas_fill_sse(canvas, YELLOW);
    end = clock();
    printf("SSE time :    %.3fms\n", (float)(end - start) * 1000.f / CLOCKS_PER_SEC);
    canvas_save_to_ppm(canvas, "sse.ppm");

    canvas_destroy(canvas);
    return 0;
}
