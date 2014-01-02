#define true 1
#define false 0

typedef unsigned char Byte;
typedef unsigned short int Word;
typedef Byte bool;
typedef Byte boolean;
typedef char* string;

// Pixel struct, not meant to be used normally.
struct Pixel {
  Byte r,g,b,a;
};

// Define this new image data type. Used to store any
// images loaded with this library
typedef struct {
  // Name of the image, not very important. Assigned to the
  // directory of the image + file + extension if loaded,
  // if a "image_new()", declared in-function.
  string name;

  // Debugging variables.
  // load_successful is true upon image loaded successfully
  // (false if no image, or image bit-depth is wrong, etc.)
  //
  // function_successful is true if your functions return image
  // is valid, false if it didn't return the modified image.
  bool load_successful;
  bool function_successful;

  // A pixel containing the color to be considered transparent
  struct Pixel transparent;

  // The width and height of the image that was loaded, used for
  // calculating x/yscale stretching.
  int original_width;
  int original_height;

  // The images current display width/height.
  int width;
  int height;

  // Stores a 2D array containing the image data dynamically
  struct Pixel** data;
} Image;

//
// IMAGE DRAWING / MANIPULATION
//

// Draws a pixel to a specific scale on a similarly scaled XY grid.
void putpixel_stretch(double x_pos, double y_pos, double x_scale, double y_scale) {
  double i;
  for (i=0; i<x_scale; i++) {
    moveto((x_pos*x_scale + i), (y_pos*y_scale));
    lineto((x_pos*x_scale + i), (y_pos*y_scale + (y_scale-1)));
  }
}

// Same as putpixel_stretch, but includes a non-scaled XY offset
void putpixel_offset(double x_pos, double y_pos, double x_scale, double y_scale, int x_offset, int y_offset) {
  double i;
  for (i=0; i<x_scale; i++) {
    moveto(x_offset + (x_pos*x_scale + i), y_offset + (y_pos*y_scale));
    lineto(x_offset + (x_pos*x_scale + i), y_offset + (y_pos*y_scale + (y_scale-1)));
  }
}

// Draws the data contained within an IMAGE array
void image_draw(Image source, int x_offset, int y_offset) {
  int i, j;
  for(i=0; i<source.original_width; i++) {
    for(j=0; j<source.original_height; j++) {
      setpen(source.data[i][j].r,
           source.data[i][j].g,
           source.data[i][j].b,
           0, 1);
      putpixel_offset(i,j,
            (double)source.width/(double)source.original_width,
            (double)source.height/(double)source.original_height,
            x_offset, y_offset);
    }
  }
}

//
// IMAGE CREATION / LOADING
//

// Creates a new image struct and allocations its memory
Image image_new(int width, int height) {
  Image new_image;
  int i;

  // Set the basic values of the struct
  new_image.width = width;
  new_image.height = height;
  new_image.original_width = width;
  new_image.original_height = height;
  new_image.transparent.r = 0;
  new_image.transparent.g = 0;
  new_image.transparent.b = 0;
  new_image.transparent.a = 0;

  new_image.data = (struct Pixel**)malloc(new_image.width * sizeof(struct Pixel*));
  for (i=0; i<new_image.width; i++) {
    new_image.data[i] = (struct Pixel*)malloc(new_image.height * sizeof(struct Pixel));
  }

  return new_image;
}

// Frees up the memory used in an image.
/* DISABLED
void image_delete(Image source) {
  int i, j;
  for (i=0; i<source.original_width; i++) {
    for(j=0; j<source.original_height; j++) {
      free(source.data[i][j]);
    }
  }
}
*/

// Takes an image, deletes its contents, and returns the modified
// struct.
/* DISABLED
Image image_resize(Image source, int width, int height) {
  // Since the image that's passed is a struct with pointers in it,
  // we don't even need to pass by reference to alter the original
  // Pixel struct.
  int i, j;

  // Delete the current contents and deallocate
  image_delete(source);

  // Allocate new 2D array
  source.data = (struct Pixel**)malloc(width * sizeof(struct Pixel*));
  for (i=0; i<width; i++) {
    source.data[i] = (struct Pixel*)malloc(height * sizeof(struct Pixel));
  }

  // Change members
  source.original_width = width;
  source.original_height = height;
  source.width = width;
  source.height = height;

  return source;
}
*/

// Loads an image, currently only .BMP files
// Currently loading BMPs incorrectly:
//   Starts at wrong origin point (rotated 90deg cw)
//   Still doesn't work on non-perfect square BMP
Image image_load(string filename) { 
  long int i, j, pad, header[55];
  Image new_image;
   
  FILE* import = fopen(filename, "r+b");

  for (i=1;i<55;i++) {
    header[i]=fgetc(import);
  }

  new_image.original_width=header[20]*256;
  new_image.original_width=new_image.original_width+header[19];
  new_image.original_height=header[24]*256;
  new_image.original_height=new_image.original_height+header[23];
  new_image.height = new_image.original_height;
  new_image.width = new_image.original_width;

  new_image.data = (struct Pixel**)malloc(new_image.width * sizeof(struct Pixel*));
  for (i=0; i<new_image.width; i++) {
    new_image.data[i] = (struct Pixel*)malloc(new_image.height * sizeof(struct Pixel));
  }

  for (i=0; i<new_image.height; i++) {
    for (j=0; j<new_image.width; j++) {
      new_image.data[i][j].b=fgetc(import);
      new_image.data[i][j].g=fgetc(import);
      new_image.data[i][j].r=fgetc(import);
    }
   
    if (fmod(new_image.width,4) > 0) {
      for (pad=0;pad<fmod(new_image.width,4);pad++) {
        fgetc(import);
      }
    }
  }
  fclose(import);
  return new_image;
}
