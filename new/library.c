//  ____ _____      _     _ _
// / ___| ____|    | |   (_) |__  ___
//| |  _|  _| _____| |   | | '_ \/ __|
//| |_| | |__|_____| |___| | |_) \__ \
// \____|_____|    |_____|_|_.__/|___/
//

// Defines
#define true 1
#define false 0

// Typedefs
typedef unsigned char Byte;
typedef unsigned short int Word;
typedef Byte bool;
typedef Byte boolean;
typedef char* string;

// (structs)
// ___                             
//|_ _|_ __ ___   __ _  __ _  ___  
// | || '_ ` _ \ / _` |/ _` |/ _ \ 
// | || | | | | | (_| | (_| |  __/ 
//|___|_| |_| |_|\__,_|\__, |\___| 
//                     |___/       
// ____  _                   _
/// ___|| |_ _ __ _   _  ___| |_ ___
//\___ \| __| '__| | | |/ __| __/ __|
// ___) | |_| |  | |_| | (__| |_\__ \
//|____/ \__|_|   \__,_|\___|\__|___/
//

// RGB pixel struct
struct Pixel {
  Byte r,g,b;
};
// Image containment struct
typedef struct image_struct {
  // Name of the image, not very important. Assigned to the
  // directory of the image + file + extension if loaded,
  // if a "image_new()", declared in-function.
  char* name;

  // A pixel containing the color to be considered transparent
  struct Pixel transparent;

  // The width and height of the image that was loaded, used for
  // calculating x/yscale stretching.
  int original_width;
  int original_height;

  // The images current display width/height.
  int width;
  int height;

  // Holds angle of rotation, currently only supports 90 degree
  // angles.
  int angle;

  // Holds the scale to draw the text at, seperate from width/height
  // scaling
  double scale;

  //
  // Number of characters and the ASCII code of the
  // first character.
  char characters;
  char first_character;

  // Set these colors like you would a normal GE actor
  char r,g,b;

  // Stores a 2D array containing the image data dynamically
  struct Pixel** data;
} Image;
// Animation struct,
typedef struct animation_struct {
  // Number of frames
  int frames;

  // FPS to be played at
  int fps;

  // List of images
  Image* images;
} Animation;

// (Tools)
// _____           _      
//|_   _|__   ___ | |___  
//  | |/ _ \ / _ \| / __| 
//  | | (_) | (_) | \__ \ 
//  |_|\___/ \___/|_|___/ 
//                        
// Collection of functions for generic usage

char * strInsert (char* txt, int pos, char* in) {
    char firsthalf[255], secondhalf[255];
    int i;
    strncpy(firsthalf, txt, pos);

    // And now loop through the rest and add it to
    // secondhalf
    for (i=0; i<(strlen(txt)-pos); i++) {
        secondhalf[i] = txt[i+pos];
    }

    // Append the character to firsthalf
    strcat(firsthalf, in);

    // Reappend secondhalf
    strcat(firsthalf, secondhalf);

    // And make our text the new variable.
    strcpy(txt, firsthalf);
    return txt;
}

// Checks if SOURCE ends with CHECK
int strend(char*source, char*check) {
   char*temp1;
   strncpy(temp1, source, strlen(source)-strlen(check));
   strcat(temp1, check);
   if(strcmp(source, temp1)==0) return 1;
   else return 0;
}

// Checks if SOURCE begins with CHECK
int strbeg(char*source, char*check) {
    char*temp1;
    strncpy(temp1, source, strlen(check));
    if(!strcmp(temp1, check)) return 1;
    else return 0;
}

// (image drawing)
// ___                             
//|_ _|_ __ ___   __ _  __ _  ___  
// | || '_ ` _ \ / _` |/ _` |/ _ \ 
// | || | | | | | (_| | (_| |  __/ 
//|___|_| |_| |_|\__,_|\__, |\___| 
//                     |___/       
// ____                     _
//|  _ \ _ __ __ ___      _(_)_ __   __ _
//| | | | '__/ _` \ \ /\ / / | '_ \ / _` |
//| |_| | | | (_| |\ V  V /| | | | | (_| |
//|____/|_|  \__,_| \_/\_/ |_|_| |_|\__, |
//                                  |___/

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

// This function draws any section of an IMAGE
// struct given several different properties.
//
// If the structs width and height have been modified, we draw it
// using a special pixel-stretching function, instead of putpixel();
//
// The RGB of each pixel is also modified by the sources r, g, and b members.
// This functionality seems to be a bit broken right now, though
void subimage_draw(Image source, int x_1, int y_1, int x_2, int y_2, int x_offset, int y_offset) {
  int i, j;
  int x1 = min(x_1, x_2), x2 = max(x_1, x_2);
  int y1 = min(y_1, y_2), y2 = max(y_1, y_2);
  int drawmode = 1;

  // Determine if we need to use a scaled putpixel function
  if(source.width == source.original_width &&
      source.height == source.original_height) {
    drawmode=2;
  }

  for(i=x1; i<x2; i++) {
    for(j=y1; j<y2; j++) {
      if(source.data[i][j].r != source.transparent.r &&
          source.data[i][j].g != source.transparent.g &&
          source.data[i][j].b != source.transparent.b) {
        struct Pixel color = source.data[i][j];
        color.r = min(color.r, color.r - (255-(source.r)));
        color.g = min(color.g, color.g - (255-(source.g)));
        color.b = min(color.b, color.b - (255-(source.b)));
        setpen(color.r, color.g, color.b, 0, 1);

        switch(drawmode) {
          case 1:
            putpixel(i-x1+x_offset, j-y1+y_offset);
            break;

          case 2:
            putpixel_offset(i-x1, j-y1,
              (double)source.width/(double)source.original_width,
              (double)source.height/(double)source.original_height,
              x_offset, y_offset);
            break;
        }
      }
    }
  }
}

// This function simply calls subimage_draw, but with
// the entire image selected.
void image_draw(Image source, int x_offset, int y_offset) {
  subimage_draw(source, 0,0, source.original_width, source.original_height, x_offset, y_offset);
}

// Takes image struct to use as font and draws text based
// upon it with an XY offset
void text_draw_offset(Image font, char str[], int x_offset, int y_offset) {
  int x_pos=0, y_pos=0, i;
  int char_width, char_height;

  // Figure out font properties
  char_width = font.original_width / font.characters;
  char_height = font.original_height;

  for(i=0; i<strlen(str); i++) {
    switch (str[i]) {
      case '\n':
        x_pos = 0;
        y_pos += 1;
        break;

      default:
        // Any normal letter
        subimage_draw(font,
            ((str[i]-font.first_character)*char_width),0,
            ((str[i]-font.first_character)*char_width)+char_width,char_height,
            (x_pos*char_width)+x_offset,
            (y_pos*char_height)+y_offset);
        x_pos += 1;
        break;
    }
  }
}

// Calls text_draw_offset with no offset
void text_draw(Image font, char str[]) {
  text_draw_offset(font, str, 0,0);
}

// (image manipulation)
// ___                             
//|_ _|_ __ ___   __ _  __ _  ___  
// | || '_ ` _ \ / _` |/ _` |/ _ \ 
// | || | | | | | (_| | (_| |  __/ 
//|___|_| |_| |_|\__,_|\__, |\___| 
//                     |___/       
//|  \/  | __ _ _ __ (_)_ __  _   _| | __ _| |_(_) ___  _ __
//| |\/| |/ _` | '_ \| | '_ \| | | | |/ _` | __| |/ _ \| '_ \
//| |  | | (_| | | | | | |_) | |_| | | (_| | |_| | (_) | | | |
//|_|  |_|\__,_|_| |_|_| .__/ \__,_|_|\__,_|\__|_|\___/|_| |_|
//                     |_|
//
Image image_setrgb(Image source, int r, int g, int b) {
  Image new_image = source;
  new_image.r = r;
  new_image.g = g;
  new_image.b = b;
  return new_image;
}

// (image loading, creation)
// ___                             
//|_ _|_ __ ___   __ _  __ _  ___  
// | || '_ ` _ \ / _` |/ _` |/ _ \ 
// | || | | | | | (_| | (_| |  __/ 
//|___|_| |_| |_|\__,_|\__, |\___| 
//                     |___/       
// _                    _ _
//| |    ___   __ _  __| (_)_ __   __ _
//| |   / _ \ / _` |/ _` | | '_ \ / _` |
//| |__| (_) | (_| | (_| | | | | | (_| |
//|_____\___/ \__,_|\__,_|_|_| |_|\__, |
//                                |___/

// Allocates a new blank image
// For use only if you want to manipulate blank images, rather
// than load images from files.
Image image_new(Image source, int width, int height) {
  int i;
  Image new_image;

  new_image.data = (struct Pixel**)malloc(width * sizeof(struct Pixel*));
  for (i=0; i<width; i++) {
    new_image.data[i] = (struct Pixel*)malloc(height * sizeof(struct Pixel));
  }

  return new_image;
}

// Frees up the memory used in an image.
// Don't use to delete global-code images.
void image_delete(Image source) {
  int i, j;
  for (i=0; i<source.original_width; i++) {
    for(j=0; j<source.original_height; j++) {
      free(source.data[i][j]);
    }
  }
}

// Sets up font options for an Image struct
Image make_font(Image source, int num_chars, char first_char) {
  Image new_image = source;
  new_image.characters = num_chars;
  new_image.first_character = first_char;
  return new_image;
}

// Checks image file extension to decide what image loader
// to use. I.E. a ".bmp" extension will use the BMP24/32 loader.
Image image_load(char source[]) {
  Image new_image;

  // BMP LOADER
  if (strend(source, ".bmp")) {
    int bmp_bitrate, padding;
    int bmp_header[64], i,j, empty_bit;
    file*bmp_file=fopen(source, "r+b");

    for(i=0; i<64; i++) {
      bmp_header[i]=fgetc(bmp_file);
    }
    if(bmp_header[0]==66 && bmp_header[1]==77) {
      char temp[4];
      new_image.width = bmp_header[18]+bmp_header[19]*256;
      new_image.height = bmp_header[22]+bmp_header[23]*256;
      bmp_bitrate = bmp_header[28];
      padding = new_image.width % 4;

      new_image.original_width = new_image.width;
      new_image.original_height = new_image.height;
      strcpy(new_image.name, source);
      new_image.transparent.r = 0;
      new_image.transparent.g = 0;
      new_image.transparent.b = 0;
      new_image.r = 255;
      new_image.g = 255;
      new_image.b = 255;

      new_image.data = (struct pixel**)malloc(new_image.width * sizeof(struct pixel*));
      for (i=0; i<new_image.width; i++) {
        new_image.data[i] = (struct pixel*)malloc(new_image.height * sizeof(struct pixel));
      }

      fseek(bmp_file, bmp_header[10], seek_set);

      for(i=new_image.height-1; i>=0; i--)
      {
        for(j=0; j<new_image.width; j++)
        {
          new_image.data[j][i].b = fgetc(bmp_file);
          new_image.data[j][i].g = fgetc(bmp_file);
          new_image.data[j][i].r = fgetc(bmp_file);
          if(bmp_bitrate==32) {
            // skip extra bit
            fgetc(bmp_file);
          }
        }
        if(padding != 0) {
          fread(&temp, padding, 1, bmp_file);
        }
      }
    }
    fclose(bmp_file);
  }

  // TODO: TGA LOADER
  else if (strend(source, ".tga")) {
  }

  return new_image;
}
