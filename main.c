#include <stdio.h>

int main() {

    int image_width = 256;
    int image_height = 256;

    FILE *file = fopen("image.ppm", "w");

    fprintf(file, "P3\n%d %d\n255\n", image_width, image_height);

    for (int i = 0; i< image_height; i++) {
        for (int j = 0; j<image_width; j++) {
            double r = ((double) j)/(image_width-1);
            double g = ((double) i)/(image_height-1);
            double b = 10.00;

            int ir = (int) (255.999 * r);
            int ig = (int) (255.999 * g);
            int ib = (int) (255.999 * b);

            fprintf(file, "%d %d %d \n", ir,ig,ib);
        }
    }
    fclose(file);
    return 0;
}
