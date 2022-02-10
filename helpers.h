#include "bmp.h"

// OPTION structure
typedef struct {
    char name;
    char *value;
} OPTION;

extern char *help;

void applyTransforms(int optInd, OPTION options[optInd], int height, int width, RGBQUAD image[height][width], BITMAPFILEHEADER *bf, BITMAPV5HEADER *bV5, char *outFile);
