//  ____ _____      _     _ _
// / ___| ____|    | |   (_) |__  ___
//| |  _|  _| _____| |   | | '_ \/ __|
//| |_| | |__|_____| |___| | |_) \__ \
// \____|_____|    |_____|_|_.__/|___/
//

// ____       _
//|  _ \  ___| |__  _   _  __ _
//| | | |/ _ \ '_ \| | | |/ _` |
//| |_| |  __/ |_) | |_| | (_| |
//|____/ \___|_.__/ \__,_|\__, |
//                        |___/

// I wrote a relatively verbose debug system for this library.
// to create a debug monitor (prints out debug messages), do this:
//  1. Create a new image in global code
//  2. Set it up as a font.
//  3. Create a new canvas actor
//  4. Set the canvas' draw actor to:
//    draw_manage(my_font)
//
//  make sure you KNOW that your font works, I suggest using the
//  "debug.bmp" font that comes packaged with this library by using
//  the function:
//    my_image = make_font(image_load("debug.bmp"), 95, ' ');
//  TODO: Wait until "production" to package this.
//
// Redraw rates are optimized by the function itself, so you don't
// need to worry about it killing your FPS (unless you debug_push too
// often).

#define DEBUG_STACK_SIZE 10
#define DEBUG_ENABLED 1

struct {
  char active;
  char redraw;
  char format[32];
} debug_options = {DEBUG_ENABLED, 1, "%s: %s"};

typedef struct debug_state {
  char label[16];
  void (*call_back)();
} debug_state;

struct debug_part {
  debug_state state;
  char description[64];
} debug_stack[DEBUG_STACK_SIZE];

debug_state debug_error = {"error", SuspendGame};
debug_state debug_info  = {"info", NULL};
debug_state debug_warn  = {"warn", NULL};

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

  int angle; // Draw angle (only 90deg increments)

  double scale; // Draw scale (1 is default)

  char characters; // # characters in font
  char first_character; // First char in font

  short int r,g,b;

  struct Pixel** data;
} Image;

typedef struct animation_struct {
  int num_frames;
  int fps;
  Image* images;
} Animation;

//  ____ _   _ ___  
// / ___| | | |_ _| 
//| |  _| | | || |  
//| |_| | |_| || |  
// \____|\___/|___| 
// ____  _                   _        
/// ___|| |_ _ __ _   _  ___| |_ ___  
//\___ \| __| '__| | | |/ __| __/ __| 
// ___) | |_| |  | |_| | (__| |_\__ \ 
//|____/ \__|_|   \__,_|\___|\__|___/ 
//
                                    
struct {
    Image main_theme; // Main theme elements default to
    int num_elements; // Stores the current number of GUI elements
} gui_options; // Make a gui_options struct right away.

typedef struct gui {
    int owner_id; // Usually a GUI actor's ANIMPOS variable.
    int type; // Type of GUI element.
    Actor* bound_to; // A pointer to the actor that this element is bound to
    char redraw; // Lets us keep FPS up by only redrawing when we need to

    union elements { // A union to save memory space, since
                     // There's a lot of variables.
        struct button { // A clickable button
            char text[256]; // Text to be displayed by said button
        } button;

        struct toggle_button { // A clickable toggle button
            int* variable; // A pointer to the variable the button needs
            char text[256]; // The text to be displayed by said button
        } toggle_button;

        struct counter { // An increment/decrement button
            char text[256];
            int* variable;
            char left_delim; // The text for the decrementer.
            char right_delim; // Text for the incrementer
        } counter;

        struct textbox { // Fully usuable text box
            char right_delim; // Text for the "enter" button
            char caret_pos; // Stores cursor/caret position
            short int active; // Stores if it has focus
            short int caret_blink; // Manages caret blinking speed
            char letters; // Four variables to check allowed characters
            char numbers;
            char symbols;
            char whitespace;
        } textbox;

        struct bar { // Displays a bar (like a progress bar)
            double *var; // Pointer to variable to display
            double max; // The max value for the bar/variable
            char text[256];
        } bar;

        struct window { // A window container
            char text[256]; // Window Title
            char buttons[3]; // Up to 3 caption buttons in the top-right
            char active; // If window is shown at all
            char fold; // If window should be folded.
        } window;

        struct menu { // A menu container
            char active; // If menu is being shown
        } menu;

        struct tooltip { // A tooltip, duh
            int delay; // Delay in frames to start fading in
            int active; // If tooltip is active
        } tooltip;

        struct slider {
            long int* variable; // Long int to use GUI vars
            int min; // Minimum value for slider
            int max; // Maximum value for slider
            int round; // Rounding for slider value (every 2, etc.)
            char text[256]; // Displayed when not moving slider
        } slider;
    } elements;
} gui;

gui* gui_elements; // Dynamic array of GUI elements, to hold them between frames.

// (Tools)
// _____           _
//|_   _|__   ___ | |___
//  | |/ _ \ / _ \| / __|
//  | | (_) | (_) | \__ \
//  |_|\___/ \___/|_|___/
//
// Collection of functions for generic usage

// Pushes data unto the debug stack
void debug_push(debug_state state, char source[]) {
  if(debug_options.active) {
    int i;
    if(strlen(source) > 64) {
      debug_push(debug_warn, "debug_push too long");
    }
    else {
      struct debug_part new_part;
      for(i=DEBUG_STACK_SIZE-1; i>0; i--) {
        debug_stack[i] = debug_stack[i-1];
      }
      strcpy(new_part.description, source);
      new_part.state = state;
      debug_stack[0] = new_part;
      debug_options.redraw=1;
    }
  }
}

int random(int lower, int upper) {
    int v1 = max(upper, lower), v2 = min(upper, lower);
    return (rand(v1-v2)+v2);
}

char* int_to_str(int val) {
    // Creates a string using sprintf
    char buf[128];
    sprintf(buf, "%d", val);
    return buf;
}

char* float_to_str(float val) {
    // Creates a string using sprintf
    char buf[128];
    sprintf(buf, "%f", val);
    return buf;
}

void strinsert(char* dest, int pos, char* in) {
  char firsthalf[255], secondhalf[255];
  int i;

  strncpy(firsthalf, dest, pos);

  for (i=0; i<(strlen(dest)-pos); i++) {
      secondhalf[i] = dest[i+pos];
  }

  strcat(firsthalf, in);
  strcat(firsthalf, secondhalf);
  strcpy(dest, firsthalf);
}

int strend(char source[], char check[]) {
  int i;
  for(i=0; i<strlen(check); i++) {
    if(source[strlen(source)-strlen(check)+i] != check[i]) {
      return 0;
    }
  }
  return 1;
}

int strbegin(char source[], char check[]) {
  int i;
  for(i=0; i<strlen(check); i++) {
    if(source[i] != check[i]) {
      return 0;
    }
  }
  return 1;
}

char chrupper(char source) {
  if (source >= 'a' && source <= 'z') {
    return source - ('a' - 'A');
  }
  return source;
}

char chrlower(char source) {
  if(source >= 'A' && source <= 'Z') {
    return source + ('a' - 'A');
  }
  return source;
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

void putpixel_stretch(double x_pos, double y_pos, double x_scale, double y_scale) {
  double i;
  for (i=0; i<x_scale; i++) {
    moveto((x_pos*x_scale + i), (y_pos*y_scale));
    lineto((x_pos*x_scale + i), (y_pos*y_scale + (y_scale-1)));
  }
}

void putpixel_offset(double x_pos, double y_pos, double x_scale, double y_scale, int x_offset, int y_offset) {
  double i;
  for (i=0; i<x_scale; i++) {
    moveto(x_offset + (x_pos*x_scale + i), y_offset + (y_pos*y_scale));
    lineto(x_offset + (x_pos*x_scale + i), y_offset + (y_pos*y_scale + (y_scale-1)));
  }
}

// TODO: implement RGB modifiers using source.r/g/b
//       previous implementation broke.
void image_draw_section(Image source, int x_1, int y_1, int x_2, int y_2, int x_offset, int y_offset) {
  int i, j;
  int x1 = min(x_1, x_2), x2 = max(x_1, x_2);
  int y1 = min(y_1, y_2), y2 = max(y_1, y_2);
  int drawmode = 1;
  
  debug_state debug_draw = {"draw", NULL};
  debug_push(debug_draw, source.name);

  if(source.width == source.original_width &&
      source.height == source.original_height) {
    drawmode = 2;
  }

  for(i=x1; i<x2; i++) {
    for(j=y1; j<y2; j++) {
      if(source.data[i][j].r != source.transparent.r &&
         source.data[i][j].g != source.transparent.g &&
         source.data[i][j].b != source.transparent.b) {
        struct Pixel color = source.data[i][j];
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

void image_draw(Image source, int x_offset, int y_offset) {
  image_draw_section(source, 0,0, source.original_width, source.original_height, x_offset, y_offset);
}

// TODO: Add more styling methods, I.E. color code '\a898', tabs '\t', etc.
void text_draw_offset(Image font, char str[], int x_offset, int y_offset) {
  int x_pos=0, y_pos=0, i;
  int char_width, char_height;
  int reset_debug=0;

  if(debug_options.active) {
    debug_state debug_write = {"write", NULL};
    debug_push(debug_write, font.name);
    debug_options.active=0;
    reset_debug=1;
  }

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
        image_draw_section(font,
            ((str[i]-font.first_character)*char_width),0,
            ((str[i]-font.first_character)*char_width)+char_width,char_height,
            (x_pos*char_width)+x_offset,
            (y_pos*char_height)+y_offset);
        x_pos += 1;
        break;
    }
  }

  if(reset_debug) debug_options.active = 1;
}

void text_draw(Image font, char str[]) {
  text_draw_offset(font, str, 0,0);
}

// This is here instead of drawing debug by hand because
// the drawing commands call debug pushes, making it an infinite loop
// this auto-handles debug activation / deactivation for you.
//
// Of course, if your image font is broken, this won't work.
void debug_manage(Image font) {
  if(debug_options.active && debug_options.redraw) {
    int i;
    debug_options.active = 0;
    erase(0,0,0,0);

    for(i=0; i<DEBUG_STACK_SIZE; i++) {
      char buf[80];
      sprintf(buf, debug_options.format, debug_stack[i].state.label, debug_stack[i].description);
      text_draw_offset(font, buf, 0, i*font.height);
      if(debug_stack[i].state.call_back != NULL) debug_stack[i].state.call_back();
    }

    debug_options.redraw = 0;
    debug_options.active = 1;
  }
}

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

  debug_push(debug_info, "new image allocated");
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
}*/

// Sets up an image's font values
Image make_font(Image source, int num_chars, char first_char) {
  Image new_image = source;
  new_image.characters = num_chars;
  new_image.first_character = first_char;
  return new_image;
}

// At the cost of efficiency, I decided to compile all the image
// loading functions into image_load(). This shouldn't hamper the
// readability of code at all, and the loader is decided by file extension.
//
// This may make it more difficult to -write- code, since you don't know what
// file extensions are supported, however.
//
// TODO: Only BMP loading (24/32bit) is active, make more.
//       Specifically TGA.
Image image_load(char source[]) {
  Image new_image;
  debug_state debug_load = {"load", NULL};

  // BMP LOADER
  if (strend(source, ".bmp")) {
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
    fclose(bmp_file);
    debug_push(debug_load, source);
  }
  return new_image;
}

//  ____ _   _ ___  
// / ___| | | |_ _| 
//| |  _| | | || |  
//| |_| | |_| || |  
// \____|\___/|___| 
//                  

void do_gui(char* type) {
  if (animpos == 0) {
    gui new_element;
    gui_options.num_elements += 1;
    animpos = gui_options.num_elements; 
    new_element.owner_id = animpos;

    if (!strcmp(type, "button")) { 
      new_element.type = 1;
    }

    realloc(gui_elements, sizeof(gui)*gui_options.num_elements); 
    gui_elements[gui_options.numelements-1] = new_element;
  } else { 
    switch (gui_elements[animpos].type) {
      default: 
        erase(0,0,0,0);
        break;

      case 0: 
        break;
    }
  }
}
