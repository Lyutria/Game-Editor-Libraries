struct TGA_HEADER {
   char  idlength;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short width;
   short height;
   char  bitsperpixel;
   char  imagedescriptor;
};

struct PIXEL {
    unsigned char r,g,b,a;
};

struct IMAGE {
    char* name; // Original name of the image
    short int type; // Bit type, 24/32

    struct PIXEL transparent; // RGB color to be used as transparent

    int original_width; // Original image width and
    int original_height; // height

    int width; // Draw image width and
    int height; // height

    struct PIXEL** data; // Dynamic 2D array of pixels
};

typedef struct IMAGE image;

// IMAGE DRAWING FUNCTIONS
void putpixels(double X, double Y, double xScale, double yScale) {
    double i;
    for (i=0; i<xScale; i++) {
        moveto((X*xScale + i), (Y*yScale));
        lineto((X*xScale + i), (Y*yScale + (yScale-1)));
    }
}

void putpixelso(double X, double Y, double xScale, double yScale, int ix, int iy) {
    double i;
    for (i=0; i<xScale; i++) {
        moveto(ix + (X*xScale + i), iy + (Y*yScale));
        lineto(ix + (X*xScale + i), iy + (Y*yScale + (yScale-1)));
    }
}

void imageDraw(image Source, int ix, int iy) {
    int i, j;
    for(i=0; i<Source.original_width; i++) {
        for(j=0; j<Source.original_height; j++) {
            setpen(Source.data[i][j].r,
                   Source.data[i][j].g,
                   Source.data[i][j].b,
                   0, 1);
            putpixelso(i,j,
                      (double)Source.width/(double)Source.original_width,
                      (double)Source.height/(double)Source.original_height,
                      ix, iy);
        }
    }
}

image imageFlip(image IMG, char dir) {
    image NewImage = IMG;
    int i, j, k = IMG.original_width-1;
    switch (dir) {
        case 'v':
            // flip vertical
            for (i=0; i<IMG.original_width; i++) {
                k = IMG.original_height-1;
                for(j=0; j<IMG.original_height; j++) {
                    NewImage.data[i][j].r = IMG.data[i][k].r;
                    NewImage.data[i][j].g = IMG.data[i][k].g;
                    NewImage.data[i][j].b = IMG.data[i][k].b;
                    NewImage.data[i][j].a = IMG.data[i][k].a;
                    k--;
                }
            }
            break;

        case 'h':
        default:
            // Horizontal
            for (i=0; i<IMG.original_width; i++) {
                for(j=0; j<IMG.original_height; j++) {
                    NewImage.data[i][j].r = IMG.data[k][j].r;
                    NewImage.data[i][j].g = IMG.data[k][j].g;
                    NewImage.data[i][j].b = IMG.data[k][j].b;
                    NewImage.data[i][j].a = IMG.data[k][j].a;
                }
                k--;
            }
            break;
    }
    return NewImage;
}

// IMAGE OPENING FUNCTIONS

// Opens a BMP file into the IMAGE struct
// Only works for BMP files that are perfect squares.
image BMP_Open(char* dir) {
    image NewImage;
    int BmpHeader[64], i,j;
    FILE*TBMP=fopen(dir, "r+b");
    strcpy(NewImage.name, dir);

    for(i=0; i<64; i++) {
        BmpHeader[i] = fgetc(TBMP);
    }

    if(BmpHeader[0]==66 && BmpHeader[1]==77) {
        NewImage.width  = BmpHeader[18]+BmpHeader[19]*256;
        NewImage.height = BmpHeader[22]+BmpHeader[23]*256;
        NewImage.type   = BmpHeader[28];
        NewImage.transparent.r= 0;
        NewImage.transparent.b= 0;
        NewImage.transparent.g= 0;
        NewImage.transparent.a= 0;

        // Now, allocate enough data to hold the new BMP
        NewImage.data = (struct PIXEL**)malloc(NewImage.width * sizeof(struct PIXEL*));
        for (i=0; i<NewImage.width; i++) {
            NewImage.data[i] = (struct PIXEL*)malloc(NewImage.height * sizeof(struct PIXEL));
        }

        fseek(TBMP, BmpHeader[10], SEEK_SET);
        for(i=NewImage.height-1; i>=0; i--) {
            for(j=0; j<NewImage.width; j++) {
                NewImage.data[j][i].g= fgetc(TBMP);
                NewImage.data[j][i].b= fgetc(TBMP);
                NewImage.data[j][i].r= fgetc(TBMP);
                if(NewImage.type == 32) {
                    NewImage.data[j][i].a = fgetc(TBMP);
                    fseek(TBMP, NewImage.width%4, SEEK_CUR);
                }
                else {
                    NewImage.data[j][i].a = 0;
                    fseek(TBMP, NewImage.width%4, SEEK_CUR);
                }
            }
        }
    }
    fclose(TBMP);
    return NewImage;
}

// Function used for TGA loading
void MergeBytes(struct PIXEL *pixel,unsigned char *p,int bytes)
{
    if (bytes == 4) {
        pixel->r = p[2];
        pixel->g = p[1];
        pixel->b = p[0];
        pixel->a = p[3];
    } else if (bytes == 3) {
        pixel->r = p[2];
        pixel->g = p[1];
        pixel->b = p[0];
        pixel->a = 0;
    } else if (bytes == 2) {
        pixel->r = (p[1] & 0x7c) << 1;
        pixel->g = ((p[1] & 0x03) << 6) | ((p[0] & 0xe0) >> 2);
        pixel->b = (p[0] & 0x1f) << 3;
        pixel->a = (p[1] & 0x80);
    }
}

// Opens a TGA file into an image struct
// works with most TGA files (16/24/32 bit any size)
// I grabbed this code from the internet, and removed
// all the file safety / debug code.
image TGA_Open(char* dir) {
    image NewImage;
    int n=0,i,j;
    int bytes2read,skipover = 0;
    unsigned char p[5];
    FILE *fptr;
    struct TGA_HEADER header;
    struct PIXEL *pixels;

    /* Open the file */
    if ((fptr = fopen(dir, "r")) != NULL) {
        header.idlength = fgetc(fptr);
        header.colourmaptype = fgetc(fptr);
        header.datatypecode = fgetc(fptr);
        fread(&header.colourmaporigin,2,1,fptr);
        fread(&header.colourmaplength,2,1,fptr);
        header.colourmapdepth = fgetc(fptr);
        fread(&header.x_origin,2,1,fptr);
        fread(&header.y_origin,2,1,fptr);
        fread(&header.width,2,1,fptr);
        fread(&header.height,2,1,fptr);
        header.bitsperpixel = fgetc(fptr);
        header.imagedescriptor = fgetc(fptr);

        NewImage.width = header.width;
        NewImage.original_width = header.width;
        NewImage.height = header.height;
        NewImage.original_height = header.height;
        strcpy(NewImage.name, dir);
        NewImage.type = header.bitsperpixel;
        NewImage.transparent.r= 0;
        NewImage.transparent.b= 0;
        NewImage.transparent.g= 0;
        NewImage.transparent.a= 0;

        /* Allocate space for the image */
        pixels = malloc(header.width*header.height*sizeof(struct PIXEL));

        NewImage.data = (struct PIXEL**)malloc(NewImage.width * sizeof(struct PIXEL*));
        for (i=0; i<NewImage.width; i++) {
            NewImage.data[i] = (struct PIXEL*)malloc(NewImage.height * sizeof(struct PIXEL));
        }

        for (i=0;i<header.width*header.height;i++) {
           pixels[i].r = 0;
           pixels[i].g = 0;
           pixels[i].b = 0;
           pixels[i].a = 0;
        }

        /* Skip over unnecessary stuff */
        skipover += header.idlength;
        skipover += header.colourmaptype * header.colourmaplength;
        fseek(fptr,skipover,SEEK_CUR);

        /* Read the image */
        bytes2read = header.bitsperpixel / 8;
        while (n < header.width * header.height) {
            if (header.datatypecode == 2) {                     /* Uncompressed */
                fread(p,1,bytes2read,fptr);
                MergeBytes(&(pixels[n]),p,bytes2read);
                n++;
            } else if (header.datatypecode == 10) {             /* Compressed */
                fread(p,1,bytes2read+1,fptr);
                j = p[0] & 0x7f;
                MergeBytes(&(pixels[n]),&(p[1]),bytes2read);
                n++;
                if (p[0] & 0x80) {         /* RLE chunk */
                    for (i=0;i<j;i++) {
                        MergeBytes(&(pixels[n]),&(p[1]),bytes2read);
                        n++;
                    }
                } else {                   /* Normal chunk */
                    for (i=0;i<j;i++) {
                        fread(p,1,bytes2read,fptr);
                        MergeBytes(&(pixels[n]),p,bytes2read);
                        n++;
                    }
                }
            }
        }
        fclose(fptr);

        // Now convert the TGA array into
        // a 2D IMAGE array.
        // Because the original code used a 1D array, and I'm
        // too stupid to convert it.
        for(i=0; i<header.width*header.height; i++) {
            int ix, iy;
            ix = i % header.width;
            iy = (i - ix) / header.width;
            NewImage.data[ix][iy].r = pixels[i].r;
            NewImage.data[ix][iy].g = pixels[i].g;
            NewImage.data[ix][iy].b = pixels[i].b;
        }
    }
    return NewImage;
}

