#include <stdio.h>
#include "vec3.h"
#include "color.h"
int main() {

    int image_width = 256;
    int image_height = 256;

    FILE *file = fopen("image.ppm", "w");

    fprintf(file, "P3\n%d %d\n255\n", image_width, image_height);

    for (int i = 0; i< image_height; i++) {
        for (int j = 0; j<image_width; j++) {
            Color c = color(((double) j)/(image_width-1), ((double) i)/(image_height-1), 0);
            write_color(file, c);
        }
    }
    fclose(file);
    return 0;
}
