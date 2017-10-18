// C-lib: IMAGE

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

#define MAX_ANIMATION_LENGTH 64

// IMAGE FONTS follow the ISO 8859-1 and Microsoft Windows Latin-1 code list as
// found at http://ascii-code.com/
// Please follow this format for creating your fonts.
//

//  __  ___  __        __  ___  __
// /__`  |  |__) |  | /  `  |  /__`
// .__/  |  |  \ \__/ \__,  |  .__/
//

typedef struct Pixel {
  unsigned char r,g,b;
  float t;
} Pixel;

typedef struct image_struct {
  char name[64]; // Path to file from image_load()

  short int original_width;
  short int original_height;
  short int width;
  short int height;
  short int angle; // Todo
  double    scale;    // Draw scale (1 is default)

  // Monitors topleft position to start in the loaded
  // image array. Used for getting sections of images
  // without using extra memory.
  struct {
    int x;
    int y;
  } topleft;

  // State information
  short int subimage; // Keeps track of subimage nesting depth
  char      indexed;  // If image is using indexed-mode
  char      font;     // If the image has been designated as a font
  char      animation;// If the image has been designated as an animation
  short int palette_size;

  short int characters; // # characters in font
  short int first_character; // First char in font
  short int char_height; // These two lower the amount of calculations when drawing using imagefonts
  short int char_width;

  short int frames;
  short int current_frame;
  short int frame_speed[MAX_ANIMATION_LENGTH];
  float     frame_count;

  short int r,g,b;

  Pixel** data;
  Pixel*  palette;
  Pixel transparent; // Undrawn color
} Image;

// For use with image_gridsplit
// Allows you to easily split an image into precise rows&columns and then
// access them. Used heavily for drawing GUI elements (break an image into
// corners and sides to stretch)
typedef struct image_grid_struct {
  Image** image;
  unsigned short int rows;
  unsigned short int columns;
  unsigned short int row_size;
  unsigned short int column_size;
} ImageGrid;

//  __     __   __
// |  \ | /__` |__) |     /\  \ /
// |__/ | .__/ |    |___ /~~\  |
//

// Our special putpixel function.
// Every drawing function eventually reaches down to this, which is a replacement for the builtin
// putpixel().
// Draws a scaled pixel according to properties specified, allowing us to loop through all the pixels
// of an image and draw each part at scale.
//  Returns 0 if pixel wasn't drawn
//  Returns 1 on success
int putpixel_offset(double x_pos, double y_pos, double x_scale, double y_scale, double x_offset, double y_offset) {
  double x, y;
  if (x_offset+x_pos*x_scale > width || y_offset+y_pos*y_scale > height) { return 0; }

  if (x_offset+x_pos*x_scale+x_scale >= width || y_offset+y_pos*y_scale+y_scale >= height) {
    // Use putpixel instead of lineto() for accuracy on edges of canvas
    // (Lineto clips at the last pixel in width and height)
    for (x=0; x<x_scale; x++) {
      for (y=0; y<y_scale; y++) {
        if (x_offset + (x_pos*x_scale + x) > width ||
            y_offset + (y_pos*y_scale + y) > height) { continue; }
        putpixel(x_offset + (x_pos*x_scale + x),
                 y_offset + (y_pos*y_scale + y));
      }
    }
  }
  else {
    // Okay to use inaccurate (but much, much faster) drawing.
    for (x=0; x<x_scale; x++) {
      moveto(min(width,  x_offset + (x_pos*x_scale + x)),
             min(height, y_offset + (y_pos*y_scale)));
      lineto(min(width,  x_offset + (x_pos*x_scale + x)),
             min(height, y_offset + (ceil(y_pos*y_scale) + (y_scale-1))));
             // Large scaling occasionally leaves small lines of pixels undrawn when using
             // inaccurate drawing, we use ceil() to make sure it's rounded further when
             // this happens.
    }
  }

  return 1;
}

// Used to draw any portion of an image (end result of image_draw, text_draw)
//  Returns 0 on halting error
//  Returns # source pixels drawn on success
int image_draw_section(Image source, int x_1, int y_1, int x_2, int y_2, int x_offset, int y_offset) {
  const double x_scale = ((double)source.width  / (double)source.original_width)  * source.scale; // Calculate drawing scale
  const double y_scale = ((double)source.height / (double)source.original_height) * source.scale;
  const int x1 = min(x_1, x_2), x2 = max(x_1, x_2); // Make sure the lower values are the correct values
  const int y1 = min(y_1, y_2), y2 = max(y_1, y_2);
  int x, y, last_x = -1, last_y = -1;
  int drawmode = 1;
  int count = 0;

  if (source.data == NULL)         { debugf("IMAGE DRAW", "No image loaded in source"); return 0; }
  if (source.original_width  <= 0) { debugf("IMAGE DRAW", "Invalid source width");      return 0; }
  if (source.original_height <= 0) { debugf("IMAGE DRAW", "Invalid source height");     return 0; }

  // Checks if we need to use putpixel_offset.
  // Because it's heavier than builtin putpixel, it's worth
  // not using it if the image hasn't been modified at all.
  if(source.width == source.original_width &&
     source.height == source.original_height &&
     source.scale == 1) {
    // Draw pixel for pixel from source
    drawmode = 1;
  }
  else if (source.width >= source.original_width ||
           source.height >= source.original_height) {
    // Draw using draw-offset putpixel to scale pixels to the appropriate size
    drawmode = 2;
  } else {
    // Sacrifice pixels for downscale speed (don't redraw over pixels)
    drawmode = 3;
  }

  // Every image (including subimages) points to the full source image.
  // Instead of cutting down pointers for subimages, we use the topleft.x/y
  // to specify what part of the images is our subimage, and only getting
  // the pixels from that section of the source.
  for(x=x1+source.topleft.x; x<x2+source.topleft.x; x++) {
    const int x_pos = x_offset + (double)(x-x1-source.topleft.x)*x_scale;
    if  (last_x == x_pos) { continue; }
    else last_x  = x_pos;

    for(y=y1+source.topleft.y; y<y2+source.topleft.y; y++) {
      const int y_pos = y_offset + (double)(y-y1-source.topleft.y)*y_scale;
      if  (last_y == y_pos) { continue; }
      else last_y  = y_pos;

      // last_x and last_y are the last calculated X and Y positions
      // of a drawn pixel. When the next position is equal to them,
      // it is not drawn (to reduce draw steps).
      // If the pixel is transparent, then reset this so that if
      // the next pixel is at the spot, it's okay to draw.
      if(source.data[x][y].r == source.transparent.r &&
         source.data[x][y].g == source.transparent.g &&
         source.data[x][y].b == source.transparent.b) {
          last_x = -1;
          last_y = -1;
          continue;
      }


      // Set the drawing color for the current pixel to the actual color
      // modified by the Image structs RGB values, mimicing builtin actor
      // coloring of GE.
      setpen((double)source.data[x][y].r*((double)source.r/(double)255.0),
             (double)source.data[x][y].g*((double)source.g/(double)255.0),
             (double)source.data[x][y].b*((double)source.b/(double)255.0),
             source.data[x][y].t, 1);

      switch(drawmode) {
        default:
        // Same as source image, draw direct.
        case 1:
          if      (x-x1+x_offset-source.topleft.x > width)  { break; }
          else if (y-y1+y_offset-source.topleft.y > height) { break; }
          putpixel(x-x1+x_offset-source.topleft.x, y-y1+y_offset-source.topleft.y);
          break;

        // Stretched in at least one direction.
        case 2:
          putpixel_offset(x-x1-source.topleft.x, y-y1-source.topleft.y, x_scale, y_scale, x_offset, y_offset);
          break;

        // Smaller than source image, use putpixel and discard extras.
        // Good for downscaling large images.
        // Sacrifices possibly important pixels, so is only used when
        // both width and height are lower than source.
        case 3:
          if(x_offset + (double)(x-x1-source.topleft.x)*x_scale > width ||
             y_offset + (double)(y-y1-source.topleft.y)*y_scale > height) { break; }
          putpixel(x_pos, y_pos);
          break;
      }

      count++;
    }
  }

  return count;
}

// Routes you to using image_draw_section, set to draw the whole image.
//  Returns 0 on halting error
//  Returns # source pixels drawn on success
int image_draw(Image source, int x_offset, int y_offset) {
  return image_draw_section(source, 0,0, source.original_width, source.original_height, x_offset, y_offset);
}

// Iterates through the string you give it, and based on the number of characters
// in the font, their width, and other factors, obtain the image from the source
// based on the ASCII code of the letter.
//  Returns 0 on halting error
//  Returns 1 on success
int text_draw_offset(Image cfont, char* str, int x_offset, int y_offset) {
  int x_pos=0, y_pos=0, i;
  int x_move=0, y_move=0;
  int dchar_width, dchar_height;
  int ascii_code;

  if (!cfont.font)        { debugf("TEXT DRAW", "Font not initialized"); return 0; } // This image is not designated as a font
  if (cfont.data == NULL) { debugf("TEXT DRAW", "No source data");       return 0; }
  if (cfont.original_width  <= 0) { debugf("TEXT DRAW", "Invalid source width");  return 0; }
  if (cfont.original_height <= 0) { debugf("TEXT DRAW", "Invalid source height"); return 0; }

  // Process the DRAWING width of the characters, factors such as source image scale...
  // Allows you to scale up text by modifying source image.
  dchar_width  = (cfont.original_width / cfont.characters) * (((double)cfont.width/(double)cfont.original_width)*cfont.scale);
  dchar_height = (cfont.original_height) * (((double)cfont.height/(double)cfont.original_height)*cfont.scale);

  for(i=0; i<strlen(str); i++) {
    int ascii_code = str[i];

    switch (ascii_code) {
      // Moves to the next line
      case '\n':
        x_pos = 0;
        y_pos += 1;
        break;

      // Shifts the cursor position based on input
      //    \r+5-1
      //    = move 5 right and up 1
      // Either + and - sign can be omit, and will be considered "+"
      //    \r3-6
      //    = move 3 right and up 6
      case '\r':
        i++;

        // X shifting
        if (str[i] == '-' || str[i] == '+') {
          x_move = ctoi(str[i+1]);
          if (str[i] == '-') { x_move *= -1; }
          i+=2;
        }
        else {
          x_move = ctoi(str[i]);
          i+=1;
        }

        // Y shifting
        if (str[i] == '-' || str[i] == '+') {
          y_move = ctoi(str[i+1]);
          if (str[i] == '-') { y_move *= -1; }
          i+=2;
        }
        else {
          y_move = ctoi(str[i]);
          i+=1;
        }
        break;

      // Modifies the next set of drawn characters to the color set in text.
      // Based on a pattern: "\aRGB", or for example, blue: "\a009"
      case '\t': // COLOR CHANGING (T for TINT)
        cfont.r = percent_of(what_percent_of(ctoi(str[i+1]), 9), 255);
        cfont.g = percent_of(what_percent_of(ctoi(str[i+2]), 9), 255);
        cfont.b = percent_of(what_percent_of(ctoi(str[i+3]), 9), 255);
        i+=3;
        break;

      // The main use for this is to allow programmers to use letters outside of normal char range
      // (that is, 255 characters). This lets you use special characters designed into the font
      // source image.
      case '\a': // ASCII CODE (A for ASCII)
        ascii_code = ctoi(str[i+1])*100 + ctoi(str[i+2])*10 + ctoi(str[i+3]);
        i += 3;

        // Don't draw out-of-range ASCII characters (not included in font)
        if(ascii_code >= cfont.characters + cfont.first_character) { break; }

      default: // NORMAL LETTERS
        if(ascii_code < cfont.first_character) { break; } // Don't render unused escape codes.
        if((y_pos*dchar_height)+y_offset > height) { break; }
        if((x_pos*dchar_width)+x_offset  >  width) { break; }

        image_draw_section(cfont,
          ((ascii_code-cfont.first_character)*cfont.char_width),0,
          ((ascii_code-cfont.first_character)*cfont.char_width)+cfont.char_width,cfont.char_height,
          (x_pos*dchar_width)+x_offset,
          (y_pos*dchar_height)+y_offset);

        x_pos += 1;
        break;

    }
  }

  return 1;
}

int text_draw(Image font, char* str) {
  return text_draw_offset(font, str, 0,0);
}

// Custom erase function because erase() doesn't respect
// actor width & height.
void image_erase(int r, int g, int b, double t) {
  int i;
  erase(0,0,0,1);
  setpen(r,g,b,t,1);
  for (i=0; i<width; i++) {
    moveto(i,0);
    lineto(i,height);
  }
}

// This is here instead of drawing debug by hand because
// the drawing commands call debug pushes, making it an infinite loop
// this auto-handles debug activation / deactivation for you.
//
// Of course, if your image font is broken, this won't work.
#ifdef DEBUG_ENABLED
  void debug_manage(Image font) {
    if(debugger.active && debugger.redraw) {
      int i;
      char message[DEBUG_LEN + DEBUG_PROMPT_LEN + 10];
      debugger.active = 0;
      erase(0,0,0,debugger.bg_transparency);

      for(i=0; i<DEBUG_STACK_SIZE; i++) {
        if (strlen(debugger.stack[i].text) == 0) { continue; }
        sprintf(message, "%s [%s]: %s", debugger.stack[i].prompt,
                                        debugger.stack[i].source ? debugger.stack[i].source->name : "",
                                        debugger.stack[i].text);
        text_draw_offset(font, message, 0, i*font.height*font.scale);
      }

      debugger.redraw = 0;
      debugger.active = 1;
    }
  }
#else
  void debug_manage();
#endif

//                    __                 ___    __
//  |\/|  /\  |\ | | |__) |  | |     /\   |  | /  \ |\ |
//  |  | /~~\ | \| | |    \__/ |___ /~~\  |  | \__/ | \|
//

void image_free(Image* source) {
  int i;

  if (source->subimage)     { debugf("IMAGE DEL", "Can't unload subimages"); return; }
  if (source->data == NULL) { debugf("IMAGE DEL", "No image in source");     return; }

  for (i=0; i<source->original_width; i++) {
    free(source->data[i]);
  }
  free(source->data);

  sprintf(debugger.buffer, "Unloaded %dx%d", source->original_width, source->original_height);
  debugf("IMAGE FREE", debugger.buffer);
}

// Sets an images RGB values... simple shorthand.
void image_setrgb(Image*source, int r, int g, int b) {
  source->r = max(min(r,255), 0);
  source->g = max(min(g,255), 0);
  source->b = max(min(b,255), 0);
}

int pixel_compare(Pixel p1, Pixel p2) {
  if (p1.r != p2.r ||
      p1.g != p2.g ||
      p1.b != p2.b ||
      p1.t != p2.t) {
    return 0;
  }
  return 1;
}

// Converts an image to use a palette
// Can only be used on non-subimages
// TODO
int image_convert_to_indexed(Image* source) {
  int x, y;
  Pixel* palette;
  if     (source->indexed)  { return 0; }
  else if(source->subimage) { return 0; }
  else source->indexed = 1;

  palette = malloc(sizeof(Pixel));

  for (x=0; x < source->original_width; x++) {
    for (y=0; y < source->original_height; y++) {
      //
    }
  }

  return 1;
}

// Creates a new Image struct pointing to the data in a different one.
// How you manage this is on your own, but it won't memory leak because it doesn't
// allocate new memory.
//  Returns NULL on halting error
//  Returns pointer to [destination] on success
Image* image_subimage(Image source, Image* destination, int x, int y, int width, int height) {
  if (source.data == NULL)         { debugf("SUBIMAGE", "No image loaded in source");   return NULL; }
  if (source.original_width  <= 0) { debugf("SUBIMAGE", "Invalid source width");        return NULL; }
  if (source.original_height <= 0) { debugf("SUBIMAGE", "Invalid source height");       return NULL; }
  if (&source == destination)      { debugf("SUBIMAGE", "Source can't be destination"); return NULL; }
  if (destination == NULL)         { debugf("SUBIMAGE", "Destination can't be NULL");   return NULL; }
  if (x < 0) { debugf("IMAGE SUB", "Invalid X position");   return NULL; }
  if (y < 0) { debugf("IMAGE SUB", "Invalid Y position");   return NULL; }
  if (width  <= 0 || width  + x  > source.original_width)  { debugf("IMAGE SUB", "Invalid width");  return NULL; }
  if (height <= 0 || height + y  > source.original_height) { debugf("IMAGE SUB", "Invalid height"); return NULL; }

  *destination = source;
  destination->topleft.x = x + source.topleft.x;
  destination->topleft.y = y + source.topleft.y;
  destination->original_width = width;
  destination->original_height = height;
  destination->width =  destination->original_width;
  destination->height = destination->original_height;
  destination->scale = source.scale;
  destination->r = source.r;
  destination->g = source.g;
  destination->b = source.b;

  // If not 0, the image is a subimage
  // Get the depth of subimagery with this.
  destination->subimage = source.subimage + 1;
  return destination;
}

// This uses image_subimage to give a 2D dynamic array of
// equal splits of an image, useful in certain situations e.g.:
//  I have a button image that is 24x24, each 8x8 square contains
//  one section (i.e. topleft, topmid, topright, etc.) you can use
//  image_gridsplit(button, 3, 3) to get an array of these sections
//  quickly
// You have to free it when you're done with it as a sacrifice for
// the convinience of this sort of access. If you don't want that, manage
// it using image_subimage yourself.
//  Returns NULL on halting error
//  Returns pointer to [destination] on success
ImageGrid* image_gridsplit(Image source, ImageGrid* destination, int columns, int rows) {
  ImageGrid new_grid;
  int x, y;
  int part_width = floor((double)source.original_width/(double)columns);
  int part_height = floor((double)source.original_height/(double)rows);

  if (source.data == NULL)         { debugf("IMAGE SPLIT", "No image loaded in source"); return NULL; }
  if (source.original_width  <= 0) { debugf("IMAGE SPLIT", "Invalid source width");      return NULL; }
  if (source.original_height <= 0) { debugf("IMAGE SPLIT", "Invalid source height");     return NULL; }
  if (destination == NULL)         { debugf("SUBIMAGE", "Destination can't be NULL");    return NULL; }
  if (columns  <= 0) { debugf("IMAGE SPLIT", "Invalid columns #");  return NULL; }
  if (rows     <= 0) { debugf("IMAGE SPLIT", "Invalid rows #");     return NULL; }

  destination->image = (Image**)malloc(columns * sizeof(Image*));
  for (x=0; x<columns; x++) {
    destination->image[x] = (Image*)malloc(rows * sizeof(Image));
  }

  for(x=0;x<columns;x++) {
    for(y=0;y<rows;y++) {
       image_subimage(source, &destination->image[x][y], x*part_width,y*part_height, part_width,part_height);
    }
  }

  destination->rows = rows;
  destination->columns = columns;
  destination->row_size = part_height;
  destination->column_size = part_width;

  return destination;
}

void image_freegrid(ImageGrid* source) {
  int i;

  if (source->image == NULL) { debugf("IMAGE FREEGRID", "No ImageGrid in source"); return; }

  for(i=0;i<source->columns;i++) {
    free(source->image[i]);
  }
  free(source->image);
}

//                        ___    __
//  /\  |\ | |  |\/|  /\   |  | /  \ |\ |
// /~~\ | \| |  |  | /~~\  |  | \__/ | \|
//

// image_play_animation(source)
//  Returns 0 on halting error
//  Returns 1 on frames image is not drawn
//  Returns 2 on frames image is drawn
int image_play_animation(Image* source, int x, int y, int force_draw) {
  Image current_frame;

  if (source->data == NULL)         { debugf("ANIM PLAY", "No image loaded in source"); return 0; }

  if (source->frame_count >= source->frame_speed[source->current_frame]) {
    source->current_frame++;
    if (source->current_frame == source->frames) source->current_frame = 0;
    source->frame_count = 0;
    force_draw = 1;
  }
  source->frame_count += (double)GAME_FPS / real_fps;

  if (force_draw) {
    image_erase(0,0,0,.99);
    image_subimage(*source, &current_frame,
                   source->original_width/source->frames*source->current_frame,0,
                   source->original_width/source->frames,source->original_height);
    current_frame.height = source->height;
    current_frame.width  = source->width == source->original_width ? source->width/source->frames : source->width;
    image_draw(current_frame, x, y);
    return 2;
  }
  return 1;
}

//       __        __          __
// |    /  \  /\  |  \ | |\ | / _`
// |___ \__/ /~~\ |__/ | | \| \__>
//

// Allocates a new image
//  Returns NULL on halting error
//  Returns pointer to destination on success
Image* image_new(Image* destination, int width, int height) {
  int i;

  if (destination == NULL) { debugf("IMAGE NEW", "Destination can't be NULL"); return NULL; }
  if (width  <= 0)         { debugf("IMAGE NEW", "Invalid source width");     return NULL; }
  if (height <= 0)         { debugf("IMAGE NEW", "Invalid source height");    return NULL; }
  if (destination->data != NULL) {
    if (destination->subimage) {
      debugf("IMAGE NEW", "Destination has data, freeing");
      image_free(destination);
    }
  }

  destination->width           = width;
  destination->height          = height;
  destination->original_width  = destination->width;
  destination->original_height = destination->height;
  strcpy(destination->name, "");
  destination->transparent.r   = 0;
  destination->transparent.g   = 0;
  destination->transparent.b   = 0;
  destination->r               = 255;
  destination->g               = 255;
  destination->b               = 255;
  destination->topleft.x       = 0;
  destination->topleft.y       = 0;
  destination->scale           = 1;
  destination->char_height     = 0;
  destination->characters      = 0;
  destination->first_character = ' ';
  destination->char_width      = 0;
  destination->subimage        = 0;
  destination->font            = 0;

  destination->data = (struct Pixel**)malloc(destination->width * sizeof(struct Pixel*));
  for (i=0; i<destination->width; i++) {
    destination->data[i] = (struct Pixel*)malloc(destination->height * sizeof(struct Pixel));
  }
  sprintf(debugger.buffer, "Created %dx%d", destination->width, destination->original_height);
  debugf("IMAGE NEW", debugger.buffer);
  return destination;
}

// Used to set an Image struct up to work as a font
int image_make_font(Image *destination, int num_chars, char first_char) {
  destination->characters  = num_chars;
  destination->first_character = first_char;
  destination->char_width  = (destination->original_width / destination->characters);
  destination->char_height = (destination->original_height);
  destination->font        = 1;
  return 1;
}

// Loads a BMP image into an existing struct
//  Returns 0 on halting error
//  Returns pointer to destination on success
Image* bmp_load(char source[], Image* destination) {
  int bmp_bitrate, padding;
  int bmp_header[64], i,j, empty_bit;
  FILE*bmp_file;

  if (strlen(source) == 0) { debugf("BMP LOAD", "BMP no source"); return NULL; }

  bmp_file=fopen(source, "r+b");

  if (!bmp_file) { debugf("BMP LOAD", "File doesn't exist"); return NULL; }

  if (destination->data != NULL) {
    if (destination->subimage) {
      debugf("BMP LOAD", "Destination has data, freeing");
      image_free(destination);
    }
  }

  for(i=0; i<64; i++) {
    bmp_header[i]=fgetc(bmp_file);
  }

  if(bmp_header[0]==66 && bmp_header[1]==77) {
    char temp[4];
    destination->width = bmp_header[18]+bmp_header[19]*256;
    destination->height = bmp_header[22]+bmp_header[23]*256;
    bmp_bitrate = bmp_header[28];
    padding = destination->width % 4;

    destination->original_width = destination->width;
    destination->original_height = destination->height;
    strcpy(destination->name, source);
    destination->transparent.r   = 0;
    destination->transparent.g   = 0;
    destination->transparent.b   = 0;
    destination->r               = 255;
    destination->g               = 255;
    destination->b               = 255;
    destination->topleft.x       = 0;
    destination->topleft.y       = 0;
    destination->scale           = 1;
    destination->char_height     = 0;
    destination->characters      = 0;
    destination->first_character = ' ';
    destination->char_width      = 0;
    destination->subimage     = 0;

    destination->data = (Pixel**)malloc(destination->width * sizeof(Pixel*));
    for (i=0; i<destination->width; i++) {
      destination->data[i] = (Pixel*)malloc(destination->height * sizeof(Pixel));
    }

    fseek(bmp_file, bmp_header[10], SEEK_SET);

    for(i=destination->height-1; i>=0; i--) {
      for(j=0; j<destination->width; j++) {
        destination->data[j][i].b = fgetc(bmp_file);
        destination->data[j][i].g = fgetc(bmp_file);
        destination->data[j][i].r = fgetc(bmp_file);
        destination->data[j][i].t = 0;
        if(bmp_bitrate==32) {
          // skip extra bit
          fgetc(bmp_file);
        }
      }
      if(padding != 0) fread(&temp, padding, 1, bmp_file);
    }
  }

  sprintf(debugger.buffer, "\"%s\"", source);
  debugf("BMP LOAD", debugger.buffer);

  fclose(bmp_file);
  return destination;
}



