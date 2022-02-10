#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define GetBit(data, n)    (data >> n) & 1     
#define SetBit(data, n)    (data |= (1 << n))
#define ClearBit(data, n)  (data &= ~(1 << n))

int  san          (double val);
void inverse      (int height, int width, RGBQUAD image[height][width]);
void logTrans     (int height, int width, RGBQUAD image[height][width], float C);
void invLogTrans  (int height, int width, RGBQUAD image[height][width], float C);
void powTrans     (int height, int width, RGBQUAD image[height][width], float C);
int  grey         (int height, int width, RGBQUAD image[height][width], BITMAPFILEHEADER *bf, BITMAPV5HEADER *bV5, char *outFile);
int  bitPlane     (int height, int width, RGBQUAD image[height][width], BITMAPFILEHEADER *bf, BITMAPV5HEADER *bV5, char *outFile);
void levelSlicing (int height, int width, RGBQUAD image[height][width], char *args);

// Help message
char *help = (
    "Usage: %s [options] <input file> <output file>\n"
    "\n"
    "   A basic command-line image processing scirpt for bitmap images.\n"
    "   Highly inspired by Harvard's CS50x (https://cs50.harvard.edu/x).\n"
    "   Developed by Imaginelenses (https://github.com/imaginelenses/transform).\n"
    "\n"
    "Options: \n"
    "  -h                   Show this message.\n"
    "  -i                   Invert colors of image.\n"
    "  -l  <factor>         Apply log gamma transform (natural log) with a\n"
    "                       multiplication <factor>.\n"
    "  -e  <factor>         Apply inverse log gamma transform (natural log)\n"
    "                       with a multiplication <factor>.\n"
    "  -p  <gamma>          Apply power-law gamma transform - RGB intensities\n"
    "                       are raised to the power (1 / <gamma>).\n"
    "                       To apply the inverse gamma transform use\n"
    "                       (1 / original gamma) as <gamma> value.\n"
    "                       Example: To get inverse of <gamma> = 2.0 set\n"
    "                       <gamma> = 0.5\n"
    "  -g                   Convert image to 8-bit greyscale image.\n"
    "                       Note: Any options provided after this will be\n"
    "                       discarded.\n"
    "  -b                   Bit plane slicing:\n"
    "                       Convert image to greyscale and generate 8 1-bit\n"
    "                       monochrome images representing each bit plane\n"
    "                       ranging from MSB (Most Significant Bit) to\n"
    "                       LSB (Least Significant Bit).\n"
    "                       Note: Any options provided after this will be\n"
    "                       discarded.\n"
    "  -s[=ATTR_LIST]       Set a range of RGB intensities to specified intensity.\n"
    "                       Attributes: \"Ra, Rb, Ri, Ga, Gb, Gi, Ba, Bb, Bi\"\n"
    "                       Intensities 'a' to 'b' are set to intensity 'i'.\n"
    "\n"
    "Note:\n"
    "   The order in which the options are provided are the order in which the\n"
    "   transforms are applied.\n"
    "\n"
);


// Apply transforms
void applyTransforms(int optInd, OPTION options[optInd], int height, 
    int width, RGBQUAD image[height][width], BITMAPFILEHEADER* bf, 
    BITMAPV5HEADER* bV5, char* outFile){
    for (int i = 0; i < optInd; i++) {
        switch (options[i].name) {
            case 'i':
                inverse       (height, width, image);
                break;
            case 'l':
                logTrans      (height, width, image, atof(options[i].value));
                break;
            case 'e':
                invLogTrans   (height, width, image, atof(options[i].value));
                break;
            case 'p':
                powTrans      (height, width, image, atof(options[i].value));
                break;
            case 'g':
                exit(grey     (height, width, image, bf, bV5, outFile));
            case 'b':
                exit(bitPlane (height, width, image, bf, bV5, outFile));
            case 's':
                levelSlicing  (height, width, image, options[i].value);
                break;
        }
    }
}

// Image inverse
void inverse(int height, int width, RGBQUAD image[height][width]) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j].rgbRed   = san(255 - image[i][j].rgbRed);
            image[i][j].rgbGreen = san(255 - image[i][j].rgbGreen);
            image[i][j].rgbBlue  = san(255 - image[i][j].rgbBlue);
        }
    }
}

// Log transform 
void logTrans(int height, int width, RGBQUAD image[height][width], float C) {
    C = C <= 0 ? 1 : C;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j].rgbRed   = san(C * log(1 + image[i][j].rgbRed));
            image[i][j].rgbGreen = san(C * log(1 + image[i][j].rgbGreen));
            image[i][j].rgbBlue  = san(C * log(1 + image[i][j].rgbBlue));
        }
    }
}

// Inverse log transform
void invLogTrans(int height, int width, RGBQUAD image[height][width], float C) {
    C = C <= 0 ? 1 : C;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j].rgbRed   = san(C * exp(1 + image[i][j].rgbRed));
            image[i][j].rgbGreen = san(C * exp(1 + image[i][j].rgbGreen));
            image[i][j].rgbBlue  = san(C * exp(1 + image[i][j].rgbBlue));
        }
    }
}

// Power-Law Transform or Gamma Transform
void powTrans(int height, int width, RGBQUAD image[height][width], float C) {
    C = C <= 0 ? 1 : C;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j].rgbRed   = san(pow(image[i][j].rgbRed,   1 / C));
            image[i][j].rgbGreen = san(pow(image[i][j].rgbGreen, 1 / C));
            image[i][j].rgbBlue  = san(pow(image[i][j].rgbBlue,  1 / C));
        }
    }
}

// Greyscale 
int grey(int height, int width, RGBQUAD image[height][width],
    BITMAPFILEHEADER *bf, BITMAPV5HEADER *bV5, char *outFile) {

    // Pixel size
    int pixelSize = bV5->bV5BitCount / 8;

    // Update file header
    bf->bfSize = bf->bfSize - (height * width * pixelSize) + (height * width * 1) + (4 * 256 * 8);
    bf->bfOffBits = bf->bfOffBits + (sizeof(RGBQUAD) * 256);

    // Update info header
    bV5->bV5BitCount = 8;
    bV5->bV5Compression = 0x0000;
    bV5->bV5ClrUsed = 256;
    bV5->bV5ClrImportant = 256;
    
    // Open output file
    FILE *outPtr = fopen(outFile, "w");
    if (outPtr == NULL) {
        fprintf(stderr, "Couldn't open %s. \n", outFile);
        return 7;
    }

    // Write headers
    fwrite(bf, sizeof(BITMAPFILEHEADER), 1, outPtr);
    fwrite(bV5, bV5->bV5Size, 1, outPtr);

    // Write color palette
    RGBQUAD bmiColors[255];
    for (int i = 0; i < 256; i++) {
        bmiColors[i].rgbRed   = i;
        bmiColors[i].rgbGreen = i;
        bmiColors[i].rgbBlue  = i;
    }
    fwrite(&bmiColors, sizeof(RGBQUAD), 256, outPtr);


    // Determine new padding for scanlines
    int padding = (4 - ((width * 1) % 4)) % 4;

    // Write image data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            BYTE pixel = san((image[i][j].rgbRed + image[i][j].rgbGreen + image[i][j].rgbGreen) / 3);
            fputc(pixel, outPtr);
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

// Bit plane slicing
int bitPlane(int height, int width, RGBQUAD image[height][width],
    BITMAPFILEHEADER *bf, BITMAPV5HEADER *bV5, char *outFile) {
    
    // Create copy of headers to create greyscale image at the end
    BITMAPFILEHEADER copy_bf = *bf;
    BITMAPV5HEADER copy_bV5 = *bV5;

    // Pixel size
    int pixelSize = bV5->bV5BitCount / 8;

    // Update file header
    bf->bfSize = bf->bfSize - (height * width * pixelSize) + (height * width * 1) + (4 * 256 * 8);
    bf->bfOffBits = bf->bfOffBits + (sizeof(RGBQUAD) * 2);

    // Update info header
    bV5->bV5BitCount = 1;
    bV5->bV5Compression = 0x0000;
    bV5->bV5ClrUsed = 2;
    bV5->bV5ClrImportant = 2;

    // Color palette
    RGBQUAD bmiColors[2];
    bmiColors[0].rgbRed   = 0;
    bmiColors[0].rgbGreen = 0;
    bmiColors[0].rgbBlue  = 0;
    
    bmiColors[1].rgbRed   = 255;
    bmiColors[1].rgbGreen = 255;
    bmiColors[1].rgbBlue  = 255;

    // Determine new padding for scanlines
    int padding = (4 - ((width / 8) % 4)) % 4;

    // Write 8 1-bit images
    for (int m = 7; m > -1; m--) {

        // File name
        char filename[20 + strlen(outFile)];
        sprintf(filename, "bitPlane%d_%s", m, outFile);

        // Open output file
        FILE *outPtr = fopen(filename, "w");
        if (outPtr == NULL) {
            fprintf(stderr, "Couldn't open %s. \n", outFile);
            return 7;
        }

        // Write headers
        fwrite(bf, sizeof(BITMAPFILEHEADER), 1, outPtr);
        fwrite(bV5, bV5->bV5Size, 1, outPtr);

        // Write color palette
        fwrite(&bmiColors, sizeof(RGBQUAD), 2, outPtr);

        // Write image data
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width - (width % 8); j = j + 8) {
                // Write a byte of data (8 pixels)
                BYTE bytePixel = 0;
                for (int k = 0; k < 8; k++) {
                    // Convert to greyscale
                    BYTE pixel = san((image[i][j + k].rgbRed + image[i][j + k].rgbGreen + image[i][j + k].rgbGreen) / 3);
                    if (GetBit(pixel, m)) {
                        SetBit(bytePixel, k);
                    }
                }
                fputc(bytePixel, outPtr);
            }
            
            // Write padding
            for (int j = 0; j < padding; j++) {
                fputc(0x00, outPtr);
            }
        }

        // Close output file
        fclose(outPtr);  
    }

    // Generate greyscale image (grey() frees image)
    grey(height, width, image, &copy_bf, &copy_bV5, outFile);

    return 0;
}

// Level Slicing
void levelSlicing(int height, int width, RGBQUAD image[height][width], char *args) {
    
    /**
     * Convert CSV to int array
     * Ra, Rb, Ri, Ga, Gb, Gi, Ba, Bb, Bi
     * Intensities a to b set to intensity i
     * 
     */
    int val[9] = {0};
    int index = 0;
    for (char *p = strtok(args, ","); p != NULL; p = strtok(NULL, ",")) {
        val[index] = san(atoi(p));
        index++;
        if (index > 8) break;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (image[i][j].rgbRed   >= val[0] && image[i][j].rgbRed   <= val[1]) {
                image[i][j].rgbRed   =  val[2];
            }
            if (image[i][j].rgbGreen >= val[3] && image[i][j].rgbGreen <= val[4]) {
                image[i][j].rgbGreen =  val[5];
            }
            if (image[i][j].rgbBlue  >= val[6] && image[i][j].rgbBlue  <= val[7]) {
                image[i][j].rgbBlue  =  val[8];
            }
        }
    }
}

// Sanitize value
int san(double val) {
    if (val < 0) {
        return 0;
    }
    return val > 255 ? 255 : (int)val;
}