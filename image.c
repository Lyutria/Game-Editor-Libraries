// PRE-REQUISITES:
// > main.c

#ifndef GELIB_MAIN
  #error This code requires MAIN.C to be included beforehand.
#else
  #ifndef GELIB_IMAGE
    #define GELIB_IMAGE
  #else
    #error This file already exists in global code.
  #endif
#endif

// IMAGE FONTS follow the ISO 8859-1 and Microsoft Windows Latin-1 code list as
// found at http://ascii-code.com/
// Please follow this format for creating your fonts.
//
// (structs)
// ____  _                   _
/// ___|| |_ _ __ _   _  ___| |_ ___
//\___ \| __| '__| | | |/ __| __/ __|
// ___) | |_| |  | |_| | (__| |_\__ \
//|____/ \__|_|   \__,_|\___|\__|___/
//

struct Pixel {
  unsigned char r,g,b;
};

typedef struct image_struct {
  char name[256]; // Path to file from image_load()

  struct Pixel transparent; // Undrawn color

  int original_width;
  int original_height;
  int width;
  int height;

  struct {
    int x;
    int y;
  } topleft;

  int angle; // Draw angle (only 90deg increments)
  int fix_canvas; // If 1, use slow putpixel() instead of lineto() to fix canvas edge drawing.

  double scale; // Draw scale (1 is default)

  int characters; // # characters in font
  char first_character; // First char in font
  int char_height;
  int char_width;

  short int r,g,b;

  struct Pixel** data;
} Image;

// For use with image_gridsplit
typedef struct image_grid_struct {
  Image** image;
  int rows;
  int columns;
  int row_size;
  int column_size;
} ImageGrid;

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

void putpixel_offset(double x_pos, double y_pos, double x_scale, double y_scale, int x_offset, int y_offset, int accurate) {
  double i,j;
  if (x_offset+x_pos*x_scale <= width && y_offset+y_pos*y_scale <= height) {
    switch(accurate) {
      case 1:
        for (i=0; i<x_scale; i++) {
          for (j=0; j<y_scale; j++) {
            putpixel(x_offset + (x_pos*x_scale + i), y_offset + (y_pos*y_scale + j));
          }
        }
        break;
      default:
        for (i=0; i<x_scale; i++) {
          moveto(x_offset + (x_pos*x_scale + i), y_offset + (y_pos*y_scale));
          lineto(x_offset + (x_pos*x_scale + i), y_offset + (y_pos*y_scale + (y_scale-1)));
        }
        break;
    }
  }
}

void image_draw_section(Image source, int x_1, int y_1, int x_2, int y_2, int x_offset, int y_offset) {
  int i, j;
  int x1 = min(x_1, x_2), x2 = max(x_1, x_2);
  int y1 = min(y_1, y_2), y2 = max(y_1, y_2);
  int drawmode = 1;

  if(source.width == source.original_width &&
     source.height == source.original_height &&
     source.scale == 1) {
    drawmode=1;
  }
  else {
    drawmode=2;
  }

  for(i=x1+source.topleft.x; i<x2+source.topleft.x; i++) {
    for(j=y1+source.topleft.y; j<y2+source.topleft.y; j++) {
      if(source.data[i][j].r != source.transparent.r ||
         source.data[i][j].g != source.transparent.g ||
         source.data[i][j].b != source.transparent.b) {

        setpen((double)source.data[i][j].r*((double)source.r/(double)255.0),
               (double)source.data[i][j].g*((double)source.g/(double)255.0),
               (double)source.data[i][j].b*((double)source.b/(double)255.0), 0, 1);

        switch(drawmode) {
          case 1:
            if(i-x1+x_offset-source.topleft.x > width) break;
            else if(j-y1+y_offset-source.topleft.y > height) break;
            putpixel(i-x1+x_offset-source.topleft.x, j-y1+y_offset-source.topleft.y);
            break;

          case 2:
            putpixel_offset(i-x1-source.topleft.x, j-y1-source.topleft.y,
              ((double)source.width/(double)source.original_width)*source.scale,
              ((double)source.height/(double)source.original_height)*source.scale,
              x_offset, y_offset, source.fix_canvas);
            break;
        }
      }
    }
  }
}

void image_draw(Image source, int x_offset, int y_offset) {
  image_draw_section(source, 0,0, source.original_width, source.original_height, x_offset, y_offset);
}

// TODO: Add more styling methods, I.E. color code '\a898', tabs '\t', etc.
void text_draw_offset(Image cfont, char str[], int x_offset, int y_offset) {
  int x_pos=0, y_pos=0, i;
  int dchar_width, dchar_height;
  int ascii_code, or, og, ob;
  or = cfont.r;
  og = cfont.g;
  ob = cfont.b;

  dchar_width  = (cfont.original_width / cfont.characters) * (((double)cfont.width/(double)cfont.original_width)*cfont.scale);
  dchar_height = (cfont.original_height) * (((double)cfont.height/(double)cfont.original_height)*cfont.scale);

  for(i=0; i<strlen(str); i++) {
    switch (str[i]) {
      case '\n': // NEW LINE
        x_pos = 0;
        y_pos += 1;
        break;

      case '\a': // ASCII CODE (A for ASCII)
        ascii_code = ctoi(str[i+1])*100 + ctoi(str[i+2])*10 + ctoi(str[i+3]);

        // If code is out of range break
        // and continue writing
        if(ascii_code > cfont.characters + cfont.first_character - 1) {
          i+=3;
          break;
        }

        image_draw_section(cfont,
            ((ascii_code-cfont.first_character)*cfont.char_width),0,
            ((ascii_code-cfont.first_character)*cfont.char_width)+cfont.char_width,cfont.char_height,
            (x_pos*dchar_width)+x_offset,
            (y_pos*dchar_height)+y_offset);
        i+=3;
        x_pos += 1;
        break;

      case '\t': // COLOR CHANGING (T for TINT)
        cfont.r = percent_of(what_percent_of(ctoi(str[i+1]), 9), 255);
        cfont.g = percent_of(what_percent_of(ctoi(str[i+2]), 9), 255);
        cfont.b = percent_of(what_percent_of(ctoi(str[i+3]), 9), 255);
        i+=3;
        break;

      default: // NORMAL LETTERS
        if(str[i] < cfont.first_character) {
          // Don't render unused escape codes,
          // etc.
          break;
        }
        image_draw_section(cfont,
            ((str[i]-cfont.first_character)*cfont.char_width),0,
            ((str[i]-cfont.first_character)*cfont.char_width)+cfont.char_width,cfont.char_height,
            (x_pos*dchar_width)+x_offset,
            (y_pos*dchar_height)+y_offset);
        x_pos += 1;
        break;
    }
  }
  cfont.r = or;
  cfont.g = og;
  cfont.b = ob;
}

void text_draw(Image font, char str[]) {
  text_draw_offset(font, str, 0,0);
}

// This is here instead of drawing debug by hand because
// the drawing commands call debug pushes, making it an infinite loop
// this auto-handles debug activation / deactivation for you.
//
// Of course, if your image font is broken, this won't work.
#ifdef DEBUG_ENABLED
  void debug_manage(Image font) {
    if(DBO.active && DBO.redraw) {
      int i;
      DBO.active = 0;
      erase(0,0,0,DBO.bg_transparency);

      for(i=0; i<DEBUG_STACK_SIZE; i++) {
        text_draw_offset(font, DBO.stack[i], 0, i*font.height*font.scale);
      }

      DBO.redraw = 0;
      DBO.active = 1;
    }
  }
#else
  void debug_manage();
#endif

// (image manipulation)
// ___
//|_ _|_ __ ___   __ _  __ _  ___
// | || '_ ` _ \ / _` |/ _` |/ _ \
// | || | | | | | (_| | (_| |  __/
//|___|_| |_| |_|\__,_|\__, |\___|
// __  __             _|___/        _       _   _
//|  \/  | __ _ _ __ (_)_ __  _   _| | __ _| |_(_) ___  _ __
//| |\/| |/ _` | '_ \| | '_ \| | | | |/ _` | __| |/ _ \| '_ \
//| |  | | (_| | | | | | |_) | |_| | | (_| | |_| | (_) | | | |
//|_|  |_|\__,_|_| |_|_| .__/ \__,_|_|\__,_|\__|_|\___/|_| |_|
//                     |_|
//

// IMPORTANT-ish:
// There's not many functions in this section. Why?
// I found it more useful to have code function in a lossless-state. All the manipulation is
// truely handled during the draw-state using variables from the Image struct (I.E. Angle, rgb, scale).
// This is so the image can be easily modified on the fly with code, and far more readable.

void image_setrgb(Image*source, int r, int g, int b) {
  source->r = max(min(r,255), 0);
  source->g = max(min(g,255), 0);
  source->b = max(min(b,255), 0);
}

Image image_subimage(Image source, int X, int Y, int width, int height) {
  Image new_image = source;
  new_image.topleft.x = X + source.topleft.x;
  new_image.topleft.y = Y + source.topleft.y;
  new_image.original_width = width;
  new_image.original_height = height;
  new_image.width = new_image.original_width;
  new_image.height = new_image.original_height;
  new_image.scale = source.scale;
  new_image.fix_canvas = source.fix_canvas;
  new_image.r = source.r;
  new_image.g = source.g;
  new_image.b = source.b;
  return new_image;
}

// This uses image_subimage to give a 2D dynamic array of
// equal splits of an image, useful in certain situations e.g.:
//  I have a button image that is 24x24, each 8x8 square contains
//  one section (i.e. topleft, topmid, topright, etc.) you can use
//  image_gridsplit(button, 3, 3) to get an array of these sections
//  quickly
ImageGrid image_gridsplit(Image source, int columns, int rows) {
  ImageGrid new_grid;
  int i, j;
  int ptw = (double)source.original_width/(double)columns;
  int pth = (double)source.original_height/(double)rows;

  new_grid.image = (Image**)malloc(columns * sizeof(Image*));
  for (i=0; i<columns; i++) {
    new_grid.image[i] = (Image*)malloc(rows * sizeof(Image));
  }

  for(i=0;i<columns;i++) {
    for(j=0;j<rows;j++) {
      new_grid.image[i][j] = image_subimage(source, i*ptw,j*pth, ptw,pth);
    }
  }

  new_grid.rows = rows;
  new_grid.columns = columns;
  new_grid.row_size = pth;
  new_grid.column_size = ptw;

  return new_grid;
}

void image_freegrid(ImageGrid*source) {
  int i;
  for(i=0;i<source->columns;i++) {
    free(source->image[i]);
  }
  free(source->image);
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

// Allocates space for an image
// Live loading might cause memory leaks, not properly tested.
Image image_new(Image source, int width, int height) {
  int i;
  Image new_image;
  new_image.width = width;
  new_image.height = height;

  new_image.data = (struct Pixel**)malloc(new_image.width * sizeof(struct Pixel*));
  for (i=0; i<new_image.width; i++) {
    new_image.data[i] = (struct Pixel*)malloc(new_image.height * sizeof(struct Pixel));
  }
  return new_image;
}


/*
void image_delete(Image source) {
  int i, j;
  for (i=0; i<source.original_width; i++) {
    for(j=0; j<source.original_height; j++) {
      free(source.data[i][j]);
    }
  }
}
*/

// Sets up an image's font values
Image make_font(Image source, int num_chars, char first_char) {
  Image new_image = source;
  new_image.characters = num_chars;
  new_image.first_character = first_char;
  new_image.char_width  = (source.original_width / source.characters);
  new_image.char_height = (source.original_height);
  return new_image;
}

// TODO: Only BMP loading (24/32bit) is active, make more.
//       Specifically TGA.
Image bmp_load(char source[]) {
  Image new_image;

  int bmp_bitrate, padding;
  int bmp_header[64], i,j, empty_bit;
  FILE*bmp_file=fopen(source, "r+b");

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
    new_image.topleft.x = 0;
    new_image.topleft.y = 0;
    new_image.scale = 1;
    new_image.char_height=0;
    new_image.characters=0;
    new_image.first_character=' ';
    new_image.char_width=0;
    new_image.fix_canvas = 0;

    new_image.data = (struct Pixel**)malloc(new_image.width * sizeof(struct Pixel*));
    for (i=0; i<new_image.width; i++) {
      new_image.data[i] = (struct Pixel*)malloc(new_image.height * sizeof(struct Pixel));
    }

    fseek(bmp_file, bmp_header[10], SEEK_SET);

    for(i=new_image.height-1; i>=0; i--) {
      for(j=0; j<new_image.width; j++) {
        new_image.data[j][i].b = fgetc(bmp_file);
        new_image.data[j][i].g = fgetc(bmp_file);
        new_image.data[j][i].r = fgetc(bmp_file);
        if(bmp_bitrate==32) {
          // skip extra bit
          fgetc(bmp_file);
        }
      }
      if(padding != 0) fread(&temp, padding, 1, bmp_file);
    }
  }

  sprintf(DBO.tbuffer, "BMP load \"%s\"", source);
  sdebug("IMAGE", DBO.tbuffer);

  fclose(bmp_file);
  return new_image;
}



