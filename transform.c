#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <stdbool.h>

#include "helpers.h"

int main(int argc, char *argv[]) {

    // Exit if no arguments passed
    if (argc == 1) {
        fprintf(stderr, help, argv[0]);
        return 0;
    }

    // Exit if too few arguments
    if (argc < 3) {
        fprintf(stderr, "Too few arguments. Run '%s -h' for help.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Valid options
    const char *validOptions = "hil:e:p:gbs:";

    // Selected options
    OPTION selectedOptions[3];
    int selOptInd = 0;
    int opt;
    while ((opt = getopt(argc, argv, validOptions)) != -1) {
        switch (opt) {
            case 'h':
                fprintf(stderr, help, argv[0]);
                return 0;
            case 'i':
            case 'l':
            case 'e':
            case 'p':
            case 'g':
            case 'b':
            case 's':
                selectedOptions[selOptInd].name = opt;
                selectedOptions[selOptInd].value = optarg;
                selOptInd++;
                break;
            default: /* '?' */
                fprintf(stderr, help, argv[0]);
                return -1;
        }
    }

    if (optind + 1 >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    // Remember filenames
    char *inFile = argv[optind];
    char *outFile = argv[optind + 1];

    // Open input file
    FILE *inPtr = fopen(inFile, "r");
    if (inPtr == NULL) {
        fprintf(stderr, "Couldn't open %s. \n", inFile);
        return 1;
    }

    // Read inFile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inPtr);

    // Check if inFile is bitmap image
    if (bf.bfType != 0x4d42) {
        fprintf(stderr, "Unsupported file format. \n");
        fclose(inPtr);
        return 2;
    }

    // Check size of info header (differs with version)
    int infoHeaderSize;
    fread(&infoHeaderSize, sizeof(DWORD), 1, inPtr);

    // Go back to the start of info header
    fseek(inPtr, -1 * sizeof(DWORD), SEEK_CUR);

    /**
     * Only support 24bit images or higher.
     * Do not support color palettes.
     */
    if (bf.bfOffBits - sizeof(bf) != infoHeaderSize) {
        fprintf(stderr, "Color palettes not supported.\n");
        fclose(inPtr);
        return 3;
    }

    // Read info header
    BITMAPV5HEADER bV5;
    fread(&bV5, infoHeaderSize, 1, inPtr);

    // Only support bitmaps without any compression.
    if (bV5.bV5Compression != 0 && bV5.bV5Compression != 3) {
        fprintf(stderr, "Compression not supported.\n");
        fclose(inPtr);
        return 4;
    }

    // Define the number of bytes required for each pixel.
    int pixelSize = 0;
    if (bV5.bV5BitCount == 24) {
        pixelSize = 3;
    }
    else if (bV5.bV5BitCount == 32 && bV5.bV5Compression == 3) {
        pixelSize = 4;
    }
    else {
        fprintf(stderr, "Unsupported color depth and compression");
        fclose(inPtr);
        return 5;
    }

    int height = abs(bV5.bV5Height);
    int width = abs(bV5.bV5Width);

    /** 
     * Allocate memory for image 
     * Sometimes more than what's required but only read/write what  
     * is required specified by pixel size.
     */
    RGBQUAD(*image)[width] = calloc(height, width * sizeof(RGBQUAD));
    if (image == NULL)
    {
        fprintf(stderr, "Not enough memory to store image.\n");
        fclose(inPtr);
        return 6;
    }

    // Determine padding for scanlines
    int padding = (4 - ((width * pixelSize) % 4)) % 4;

    // Read image data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fread(&image[i][j], pixelSize, 1, inPtr);
        }

        // Skip over padding
        fseek(inPtr, padding, SEEK_CUR);
    }

    // Close input file
    fclose(inPtr);

    // Apply transforms
    applyTransforms(selOptInd, selectedOptions, height, width, image, &bf, &bV5, outFile);

    // Open output file
    FILE *outPtr = fopen(outFile, "w");
    if (outPtr == NULL) {
        fprintf(stderr, "Couldn't open %s. \n", outFile);
        return 7;
    }

    // Write headers
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outPtr);
    fwrite(&bV5, infoHeaderSize, 1, outPtr);

    // Write image data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fwrite(&image[i][j], pixelSize, 1, outPtr);
        }

        // Write padding
        for (int j = 0; j < padding; j++) {
            fputc(0x00, outPtr);
        }
    }

    // Free image
    free(image);

    // Close output file
    fclose(outPtr);

    return 0;
}
