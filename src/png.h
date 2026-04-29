#ifndef RAYTRACER_PNG_H
#define RAYTRACER_PNG_H

#include "color.h"
#include <stdio.h>

void write_png(FILE *f, const Color *buffer, int width, int height);
#endif