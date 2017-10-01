// PRE-REQUISITES:
// > main.c
// > image.c
// > text.c


#if defined(GELIB_MAIN) && defined(GELIB_IMAGE) && defined(GELIB_TEXT)
  #ifndef GELIB_GUI
    #define GELIB_GUI
  #else
    #error This file already exists in global code.
  #endif
#else
  #error This code requires MAIN.C, IMAGE.C, AND TEXT.C to be included beforehand.
#endif

#define GUI_OBJECT 0
#define GUI_BUTTON 1
#define GUI_LABEL 2
#define GUI_TOGGLE_BUTTON 3
#define GUI_COUNTER 4
#define GUI_TEXTBOX 5
#define GUI_BUTTON_TOGGLE 6
#define GUI_TOOLTIP 7
#define GUI_SLIDER 8
#define GUI_METER 9
#define GUI_WINDOW 10
#define GUI_MENU 11


// ____  _                   _
/// ___|| |_ _ __ _   _  ___| |_ ___
//\___ \| __| '__| | | |/ __| __/ __|
// ___) | |_| |  | |_| | (__| |_\__ \
//|____/ \__|_|   \__,_|\___|\__|___/
//


struct {
    Image main_theme; // Main theme elements default to
    int num_elements; // Stores the current number of GUI elements
    int mouse_state;
} gui_options; // Make a gui_options struct right away.


struct gui_theme {
  Image font;
  Image button;
  // Image label;
  Image toggle_button;
  Image counter;
  Image textbox;
  Image tooltip;
  Image slider;
  Image meter;
  Image window;
  Image window_highlight;
  Image window_folded;
  Image window_captions;
  Image box;
  Image menu;
  struct {
    int x;
    int y;
  } text_offset;
} gui_theme;


typedef struct gui {
  char owner[16]; // Owner actor name
  int owner_id; // Usually a GUI actor's ANIMPOS variable
  short int type;
  char redraw; // Lets us keep FPS up by only redrawing when we need to
  char initialized;
  unsigned char r,g,b; // Colors for the element so you don't have to set it in the Image
  Image* font; // Lets different gui elements have different fonts
  struct gui_theme* theme;

  char focus;
  char disabled;

  Actor* bound_to; // A pointer to the actor that this element is bound to
  struct {
    short int l;
    short int l_distance;
    short int r;
    short int r_distance;
    short int u;
    short int u_distance;
    short int d;
    short int d_distance;
  } bound;

  union set { // A union to save memory space, since
              // There's a lot of variables.
    struct button { // A clickable button
      char text[256]; // Text to be displayed by said button
      int state; // The state of the button (mouseover, mousedown, etc.)
    } button;


    struct label {
      char text[512];
      char align;
    } label;


    struct toggle_button { // A clickable toggle button
      int* variable; // A pointer to the variable the button needs
      char text[256]; // The text to be displayed by said button
    } toggle_button;


    struct counter { // An increment/decrement button
      char text[256];
      int* variable;
      char left_delimiter; // The text for the decrementer.
      char right_delimiter; // Text for the incrementer
    } counter;


    struct textbox { // Fully usuable text box
      char* text;
      char right_delimiter; // Text for the "enter" button
      char caret_pos; // Stores cursor/caret position
      short int active; // Stores if it has focus
      short int caret_blink; // Manages caret blinking speed
      char letters; // Four variables to check allowed characters
      char numbers;
      char symbols;
      char whitespace;
    } textbox;


    struct meter { // Displays a bar (like a progress bar)
      int value; // Value to display
      int last_value;
      int margin; // Number of pixels from (BOTH) sides that the meter starts and ends
                 // (so the edges don't have to be EXACT on the edges of the image for
                 // proper calculations)
      int max; // The max value for the bar/variable
      char text[256];
    } meter;


    struct window { // A window container
      char text[256]; // Window Title
      char buttons[3]; // Up to 3 caption buttons in the top-right
      short int active; // If window is shown at all
      short int fold; // If window should be folded.
      short int foldable;
      short int closable;
      short int resizable; // default is 0, 1 is true.
      short int fold_state;
      short int close_state;
      struct {
        int x;
        int y;
      } offset;
      struct {
        int width;
        int height;
      } max;
      struct {
        int width;
        int height;
      } min;
      int state;
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
  } set;
} gui;


gui* gui_elements; // Dynamic array of GUI elements, to hold them between frames.


//  ____ _   _ ___
// / ___| | | |_ _|
//| |  _| | | || |
//| |_| | |_| || |
// \____|\___/|___|
//


void mouse_to_actor(void) {
  screen_to_actor(&xmouse, &ymouse);
}


void mouse_to_screen(void) {
  actor_to_screen(&xmouse, &ymouse);
}


int hotspot(int x1, int y1, int x2, int y2) {
  if(xmouse>x1+xscreen && ymouse>y1+yscreen && xmouse<x2+xscreen && ymouse<y2+yscreen) {
    return 1;
  }
  else {
    return 0;
  }
}

gui* this_element() {
  return &gui_elements[(int)animpos-1];
}

gui* get_element(Actor*source) {
  if (source->animpos != 0) {
    return &gui_elements[(int)source->animpos-1];
  }
  else return NULL;
}

// Bind top, bottom, left, right edges to a source actors width/height;
void bind_this_element_to(Actor*source, char dir[]) {
  gui* this_e = this_element();
  int old_x = x, old_y = y;
  int old_width = width, old_height = height;
  if ( this_e->bound_to != source ) { // setup
    int i;
    ChangeParent("Event Actor", source->name);
    for(i=0; i<strlen(dir); i++) {
      switch(dir[i]) {
        case 'b':
        case 'd':
          this_e->bound.d = 1;
          this_e->bound.d_distance = source->height - height - y;
          break;
        case 't':
        case 'u':
          this_e->bound.u = 1;
          this_e->bound.u_distance = y;
          break;
        case 'l':
          this_e->bound.l = 1;
          this_e->bound.l_distance = x;
          break;
        case 'r':
          this_e->bound.r_distance = source->width - width - x;
          this_e->bound.r = 1;
          break;
      }
    }
    this_e->bound_to = source;
  }

  if (this_e->bound.l && this_e->bound.r) {
    x = this_e->bound.l_distance;
    width = this_e->bound_to->width - this_e->bound.l_distance - this_e->bound.r_distance;
  }
  else if (this_e->bound.l) {
    x = this_e->bound.l_distance;
  }
  else if (this_e->bound.r) {
    x = this_e->bound_to->width - this_e->bound.r_distance - width;
  }

  if (this_e->bound.u && this_e->bound.d) {
    y = this_e->bound.u_distance;
    height = this_e->bound_to->height - this_e->bound.u_distance - this_e->bound.d_distance;
  }
  else if (this_e->bound.u) {
    y = this_e->bound.u_distance;
  }
  else if (this_e->bound.d) {
    y = this_e->bound_to->height - this_e->bound.d_distance - height;
  }

  if(source->animpos != 0) {
    if(get_element(source)->type == GUI_WINDOW) {
      if(get_element(source)->set.window.fold) { transp = 1; }
      else if(!get_element(source)->set.window.active) { transp = 1; }
      else { transp = 0; }
    }
  }

  if (height != old_height || width != old_width || x != old_x || y != old_y) {
    this_e->redraw =1;
  }
}

void redraw_element(Actor* source) {
  get_element(source)->redraw=1;
}

void disable_element(Actor* source) {
  get_element(source)->disabled = 1;
  get_element(source)->redraw   = 1;
}

void enable_element(Actor* source) {
  get_element(source)->disabled = 0;
  get_element(source)->redraw   = 1;
}

void redraw_all_elements() {
  int i;
  for (i=0; i<gui_options.num_elements; i++) {
    gui_elements[i].redraw = 1;
  }
}


void init_gui() {
  gui_options.num_elements=0;
  gui_options.mouse_state=0;
  gui_theme.text_offset.x=0;
  gui_theme.text_offset.y=0;
  gui_elements = malloc(sizeof(gui));
}


// This is here to lower the sheer amount of
// code for GUI drawing, as most of the elements
// use the same method
void image_draw_3x3s(Image source, int width, int height) {
  ImageGrid igrid;
  igrid = image_gridsplit(source, 3, 3);
  // We seperate the grabbed button image into 9 parts for good looking stretching
  // And now draw it

  // Draw middle
  if(width > igrid.column_size*2 && height > igrid.row_size*2) {
    igrid.image[1][1].width = width-igrid.column_size*2;
    igrid.image[1][1].height = height-igrid.row_size*2;
    image_draw(igrid.image[1][1], igrid.column_size, igrid.row_size);
  }

  // Draw sides
  igrid.image[1][2].fix_canvas = 1;
  igrid.image[1][0].width = width-(igrid.column_size*2); //top
  igrid.image[1][2].width = width-(igrid.column_size*2); //bottom
  igrid.image[2][1].fix_canvas = 1;
  igrid.image[0][1].height=height-(igrid.row_size*2); //left
  igrid.image[2][1].height=height-(igrid.row_size*2); //right
  if(width < igrid.column_size) {
    igrid.image[2][1].width = width;
    igrid.image[0][1].width = width;
  }
  if(height < igrid.row_size) {
    igrid.image[2][1].height = height;
    igrid.image[0][1].height = height;
  }
  if(height > igrid.row_size*2) {
    image_draw(igrid.image[2][1], width-igrid.column_size, igrid.row_size);
  }
  image_draw(igrid.image[0][1], 0,igrid.row_size);
  if(width > igrid.column_size*2) {
    image_draw(igrid.image[1][2], igrid.column_size,height-igrid.row_size);
  }
  image_draw(igrid.image[1][0], igrid.column_size,0);

  // Draw corners
  if(width < igrid.column_size) {
    igrid.image[0][2].width = width;
    igrid.image[0][0].width = width;
    image_draw(igrid.image[0][0], 0,0);
    image_draw(igrid.image[0][2], 0,height-igrid.row_size);
  }
  else if(height < igrid.row_size) {
    igrid.image[2][2].height=height;
    igrid.image[2][0].height=height;
    image_draw(igrid.image[2][0], width-igrid.column_size,0);
    image_draw(igrid.image[2][2], width-igrid.column_size,height-igrid.row_size);
  }
  else {
    image_draw(igrid.image[2][2], width-igrid.column_size,height-igrid.row_size);
    image_draw(igrid.image[0][2], 0,height-igrid.row_size);
    image_draw(igrid.image[2][0], width-igrid.column_size,0);
    image_draw(igrid.image[0][0], 0,0);
  }

  image_freegrid(&igrid);
}

// Draw singular zone from a 3x3
void image_draw_3x3_section(Image source, int width, int height, int zone) {
  ImageGrid igrid;
  igrid = image_gridsplit(source, 3, 3);
  // We seperate the grabbed button image into 9 parts for good looking stretching
  // And now draw it

  // Draw middle
  if(width > igrid.column_size*2 && height > igrid.row_size*2) {
    igrid.image[1][1].width = width-igrid.column_size*2;
    igrid.image[1][1].height = height-igrid.row_size*2;
    if (zone == 5) image_draw(igrid.image[1][1], igrid.column_size, igrid.row_size);
  }

  // Draw sides
  igrid.image[1][2].fix_canvas = 1;
  igrid.image[1][0].width = width-(igrid.column_size*2); //top
  igrid.image[1][2].width = width-(igrid.column_size*2); //bottom
  igrid.image[2][1].fix_canvas = 1;
  igrid.image[0][1].height=height-(igrid.row_size*2); //left
  igrid.image[2][1].height=height-(igrid.row_size*2); //right
  if(width < igrid.column_size) {
    igrid.image[2][1].width = width;
    igrid.image[0][1].width = width;
  }
  if(height < igrid.row_size*2) {
    igrid.image[2][1].height = height;
    igrid.image[0][1].height = height;
  }
  if (zone == 6) image_draw(igrid.image[2][1], width-igrid.column_size, igrid.row_size);
  if (zone == 4) image_draw(igrid.image[0][1], 0,igrid.row_size);
  if (zone == 8) image_draw(igrid.image[1][2], igrid.column_size,height-igrid.row_size);
  if (zone == 2) image_draw(igrid.image[1][0], igrid.column_size,0);

  // Draw corners
  if(width < igrid.column_size) {
    igrid.image[0][2].width = width;
    igrid.image[0][0].width = width;
    if (zone == 1) image_draw(igrid.image[0][0], 0,0);
    if (zone == 7) image_draw(igrid.image[0][2], 0,height-igrid.row_size);
  }
  else if(height < igrid.row_size) {
    igrid.image[2][2].height=height;
    igrid.image[2][0].height=height;
    if (zone == 3) image_draw(igrid.image[2][0], width-igrid.column_size,0);
    if (zone == 9) image_draw(igrid.image[2][2], width-igrid.column_size,height-igrid.row_size);
  }
  else {
    if (zone == 9) image_draw(igrid.image[2][2], width-igrid.column_size,height-igrid.row_size);
    if (zone == 7) image_draw(igrid.image[0][2], 0,height-igrid.row_size);
    if (zone == 3) image_draw(igrid.image[2][0], width-igrid.column_size,0);
    if (zone == 1) image_draw(igrid.image[0][0], 0,0);
  }

  image_freegrid(&igrid);
}

void image_draw_3x3(Image source) {
  image_draw_3x3s(source, width, height);
}

int gui_find_section(const int section_size) {
  int section = 0;
  if (!hotspot(0,0,width,height)) { return 0; } // not in actor
  mouse_to_actor();
  if (xmouse <= section_size)             { section += 1; }
  else if (xmouse >= width-section_size)  { section += 3; }
  else                                    { section += 2; }
  if (ymouse <= section_size)             { section += 0; }
  else if (ymouse >= height-section_size) { section += 6; }
  else                                    { section += 3; }
  mouse_to_screen();
  return section;
}

int do_create() {
  if(animpos != 0 && this_element()->initialized != 1) {
    this_element()->initialized = 1;
    return 1;
  }
  else return 0;
}

void do_gui(char type[]) {
  gui* this_e;

  if (animpos==0) {
    char buffers[2][256];
    int params;
    gui new_element;
    gui_options.num_elements += 1;
    animpos = gui_options.num_elements;
    new_element.owner_id = animpos;
    new_element.redraw = 1;
    new_element.theme = &gui_theme;
    new_element.font = &gui_theme.font;
    new_element.r = 255;
    new_element.g = 255;
    new_element.b = 255;
    new_element.focus = 0;
    new_element.disabled = 0;
    strncpy(new_element.owner, name, 16);

    params = sscanf(type, "%[^:]%*c%[^\n]", buffers[0], buffers[1]);

    if(!strcmp(buffers[0], "object")){
      new_element.type = GUI_OBJECT;
    }
    else if (!strcmp(buffers[0], "button")) {
      new_element.type = GUI_BUTTON;
      if (params>2) {
        sscanf(buffers[1], "%[^\n]", new_element.set.button.text);
      }
    }
    else if (!strcmp(buffers[0], "window")) {
      new_element.type = GUI_WINDOW;
      new_element.set.window.active=1;
      new_element.set.window.fold=0;
      new_element.set.window.closable=0;
      new_element.set.window.foldable=1;
      new_element.set.window.fold_state = 0;
      new_element.set.window.close_state = 0;
      new_element.set.window.max.height = height;
      new_element.set.window.max.width  = width;
      if (params>2) {
        sscanf(buffers[1], "%[^\n]", new_element.set.window.text);
      }
    }
    else if (!strcmp(buffers[0], "label")) {
      new_element.type = GUI_LABEL;
      new_element.set.label.align = 'l';
      if (params>2) {
        sscanf(buffers[1], "%[^\n]", text);
      }
    }
    else if (!strcmp(buffers[0], "menu")) {
      new_element.type = GUI_MENU;
      new_element.set.menu.active=0;
    }
    else if (!strcmp(buffers[0], "meter")) {
      new_element.type = GUI_METER;
      new_element.type = GUI_METER;
      new_element.set.meter.last_value = -1;
      sscanf(buffers[1], "%i,%i", &new_element.set.meter.value, &new_element.set.meter.max);
    }

    gui_elements = realloc(gui_elements, sizeof(gui)*gui_options.num_elements);
    gui_elements[gui_options.num_elements-1] = new_element;
    return;
  }

  this_e = this_element();
  switch(this_e->type) {
    default:
      erase(255,0,0,0);
      break;

    // GUI: OBJECT
    case GUI_OBJECT:
    { // Do nothing
      break;
    }

    // GUI: BUTTON CODE
    case GUI_BUTTON:
    { const int button_height = this_e->theme->button.original_height/4;
      const int button_width = this_e->theme->button.original_width;
      int new_state = this_e->set.button.state;
      int button_mode = this_e->set.button.state;
      Image current_button;

      if (this_e->set.button.state != 2) { // mousedown
        if   ( hotspot(0,0,width,height) ) { new_state = 1; } // mouseover
        else { new_state = 0; } // normal
      }
      else if(!hotspot(0,0,width,height)) { new_state = 0; }

      if (this_e->set.button.state != new_state) {
        this_e->set.button.state = new_state;
        this_e->redraw = 1;
      }

      if (!this_e->redraw) break;
      if (this_e->disabled) { button_mode = 4; }
      else { button_mode = new_state; }
      current_button = image_subimage(this_e->theme->button, 0,button_height*(button_mode), button_width,button_height);
      image_setrgb(&current_button, this_e->r, this_e->g, this_e->b);
      erase(0,0,0,1);
      image_draw_3x3(current_button);
      text_draw_offset(*this_e->font, this_e->set.button.text,
                        (width/2) - ((strlens(this_e->set.button.text)*(this_e->font->char_width))/2) + this_e->theme->text_offset.x,
                        (height/2)- ((this_e->font->height)/2) + this_e->theme->text_offset.y);

      this_e->redraw = 0;
      break;
    }


    // GUI: WINDOWCODE
    case GUI_WINDOW:
    { const int section_size   = (this_e->theme->window.original_height/3)/3;
      const int caption_width  = this_e->theme->window_captions.original_width/3;
      const int caption_height = this_e->theme->window_captions.original_height/3;
      const int window_height  = this_e->theme->window.original_height/3;
      const int window_width   = this_e->theme->window.original_width;
      const int fold_height    = (double)this_e->theme->window_folded.original_height/3.0;
      int fold_state  = this_e->set.window.fold_state;
      int close_state = this_e->set.window.close_state;
      int new_state   = this_e->set.window.state;
      Image current_window;

      if(!this_e->set.window.active) { erase(0,0,0,1); break; }
      if(this_e->set.window.closable && hotspot(width-caption_width,0, width,caption_height)) {
        if(this_e->set.window.close_state != 2) { close_state = 1; }
      }
      else if (close_state != 2) { close_state = 0; }

      if(this_e->set.window.foldable) {
        if (fold_state != 2) { fold_state = 0; }
        if(this_e->set.window.closable) {
          if(hotspot(width-caption_width*2,0, width-caption_width,caption_height)) {
            if(this_e->set.window.fold_state != 2) { fold_state = 1; }
          }
        }
        else if (hotspot(width-caption_width,0, width,caption_height)) {
          if(this_e->set.window.fold_state != 2) { fold_state = 1; }
        }
      }

      if(this_e->set.window.state != 2 && this_e->set.window.state < 4) { // dragged
        if(hotspot(0,0,width,height) && !hotspot(3,3,width-3,height-3) && this_e->set.window.resizable) {
          new_state = 3; // edge highlighted
        } else if(hotspot(0,0,width,section_size)) {
          new_state = 1; // mouseover
          if (fold_state || close_state) new_state = 0;
        } else {
          new_state = 0; // normal
        }
      }
      else if (new_state == 2) { // dragging
        xscreen = xmouse-this_e->set.window.offset.x;
        yscreen = ymouse-this_e->set.window.offset.y;
      }
      else if (new_state >= 4) { // resizing
        const int x_mouse = xmouse - xscreen;
        const int y_mouse = ymouse - yscreen;
        const int state = new_state-3;
        if (state == 1 || state == 2 || state == 3) {
          if (height-y_mouse > this_e->set.window.min.height) {
            if (height < this_e->set.window.max.height) { y += y_mouse; }
            height = height - y_mouse;
          }
        }
        if (state == 1 || state == 4 || state == 7) {
          if (width-x_mouse > this_e->set.window.min.width) {
            if (width < this_e->set.window.max.width) { x += x_mouse; }
            width = width - x_mouse;
          }
        }
        if (state == 3 || state == 6 || state == 9) { width = x_mouse; }
        if (state == 7 || state == 8 || state == 9) { height = y_mouse; }
        if(width > this_e->set.window.max.width)    { width = this_e->set.window.max.width; }
        if(height > this_e->set.window.max.height ) { height = this_e->set.window.max.height ; }
        if(width < this_e->set.window.min.width)    { width = this_e->set.window.min.width; }
        if(height < this_e->set.window.min.height ) { height = this_e->set.window.min.height ; }
        this_e->redraw = 1;
      }

      if(this_e->set.window.close_state != close_state) {
        this_e->set.window.close_state = close_state;
        this_e->redraw = 1;
      }
      if(this_e->set.window.fold_state != fold_state) {
        this_e->set.window.fold_state = fold_state;
        this_e->redraw = 1;
      }
      if(this_e->set.window.state != new_state) {
        this_e->set.window.state = new_state;
        this_e->redraw = 1;
      }

      if(!this_e->redraw) break;
      if(!this_e->set.window.fold) {
        current_window = image_subimage(this_e->theme->window, 0,window_height*(this_e->set.window.state > 2 ? 0: this_e->set.window.state), window_width,window_height);
        image_setrgb(&current_window, this_e->r, this_e->g, this_e->b);
        erase(0,0,0,1);
        image_draw_3x3(current_window);
      }
      else {
        erase(0,0,0,1);
        current_window = image_subimage(this_e->theme->window_folded,
                                        0,fold_height*(this_e->set.window.state > 2 ? 0: this_e->set.window.state),
                                        this_e->theme->window_folded.original_width, fold_height);
        image_draw_3x3s(current_window,width,fold_height);
      }

      // Edge highlights for resizing
      if(new_state >= 3) {
        int zone = gui_find_section(window_height/3);
        if (!this_e->set.window.fold) {
          image_draw_3x3_section(this_e->theme->window_highlight, width,height, zone);
        }
      }

      // Draw caption buttons
      if(this_e->set.window.closable) {
        Image close_button = image_subimage(this_e->theme->window_captions,
                                            0, close_state*caption_height,
                                            caption_width, caption_height);
        image_draw(close_button, width-caption_width,0);
      }
      if(this_e->set.window.foldable) {
        Image fold_button = image_subimage(this_e->theme->window_captions,
                                           caption_width*(this_e->set.window.fold+1), fold_state*caption_height,
                                           caption_width, caption_height);
        if(this_e->set.window.closable) { image_draw(fold_button, width-caption_width*2,0); }
        else { image_draw(fold_button, width-caption_width,0); }
      }

      text_draw_offset(*this_e->font, this_e->set.window.text,
                        (width/2) - ((strlens(this_e->set.window.text)*this_e->font->char_width)/2) + this_e->theme->text_offset.x,
                        (section_size/2)-((this_e->font->height)/2) + this_e->theme->text_offset.y);

      this_e->redraw = 0;
      break;
    }


    //GUI: LABELCODE
    case GUI_LABEL:
    { if(strcmp(text, this_e->set.label.text)) {
        strncpy(this_e->set.label.text, text, 512);
        this_e->redraw=1;
      }

      if(!this_e->redraw) break;
      erase(0,0,0,1);
      switch(this_e->set.label.align) {
        case 'l':
          text_draw_offset(*this_e->font, this_e->set.label.text,
                            this_e->theme->text_offset.x,
                            (height/2)- ((this_e->font->height)/2) + this_e->theme->text_offset.y);
          break;
        case 'c':
          text_draw_offset(*this_e->font, this_e->set.label.text,
                            (width/2) - ((strlens(this_e->set.label.text)*this_e->font->char_width)/2) + this_e->theme->text_offset.x,
                            (height/2)- ((this_e->font->height)/2) + this_e->theme->text_offset.y);
          break;
        case 'r':
          text_draw_offset(*this_e->font, this_e->set.label.text,
                            (width) - ((strlens(this_e->set.label.text)*this_e->font->char_width)) + this_e->theme->text_offset.x,
                            (height/2)- ((this_e->font->height)/2) + this_e->theme->text_offset.y);
          break;
      }
      this_e->redraw = 0;
      break;
    }


    //GUI: METERCODE
    case GUI_METER:
    { const int bar_length = (int)((double)width*((double)this_e->set.meter.value/(double)this_e->set.meter.max));
      const Image* cur   = &this_e->theme->meter;
      Image first_layer  = image_subimage(*cur,0,0,cur->original_width,cur->original_height/2);
      Image second_layer = image_subimage(*cur,0,cur->original_height/2,cur->original_width,cur->original_height/2);

      if(this_e->set.meter.value < 0) { this_e->set.meter.value = 0; }
      else if (this_e->set.meter.value > this_e->set.meter.max) { this_e->set.meter.value = this_e->set.meter.max; }

      if(this_e->set.meter.last_value != this_e->set.meter.value) {
        this_e->redraw=1;
      }

      if(!this_e->redraw) break;
      erase(0,0,0,1);
      image_draw_3x3(first_layer);
      if(bar_length > 0) image_draw_3x3s(second_layer, bar_length, height);
      this_e->set.meter.last_value = this_e->set.meter.value;
      this_e->redraw=0;
      break;
    }


    //GUI: MENUCODE
    case GUI_MENU:
    { if(this_e->set.menu.active) { transp=0; }
      else { transp=1; }

      if(!this_e->redraw) break;
      erase(0,0,0,1);
      image_draw_3x3(this_e->theme->menu);
      this_e->redraw=0;
      break;
    }
  }
}


void gui_mousedown() {
  int new_state;
  gui* this_e = this_element();

  switch(this_e->type) {
    case GUI_BUTTON:
    { if(this_e->disabled) break;
      this_e->set.button.state=2;
      this_e->redraw=1;
      break;
    }

    case GUI_WINDOW:
    { const int caption_width  = this_e->theme->window_captions.original_width/3;
      const int caption_height = this_e->theme->window_captions.original_height/3;
      // Edge resizing
      if(hotspot(0,0,width,height) && !hotspot(3,3,width-3,height-3) && this_e->set.window.resizable) {
        int zone = gui_find_section(this_e->theme->window_highlight.original_height/3);
        if (this_e->set.window.fold) {
          if(zone == 1 || zone == 4 || zone == 7) {
            this_e->set.window.state = 3 + 4;
            this_e->redraw = 1;
          }
          else if (zone == 3 || zone == 6 || zone == 9) {
            this_e->set.window.state = 3 + 6;
            this_e->redraw = 1;
          }
          break;
        }
        this_e->set.window.state = 3 + zone;
        this_e->redraw=1;
        break;
      }

      // Caption buttons
      if(this_e->set.window.closable && hotspot(width-caption_width,0, width,caption_height)) {
        this_e->set.window.close_state = 2;
        this_e->redraw=1;
        break;
      }
      if(this_e->set.window.foldable) {
        if(this_e->set.window.closable) {
          if(hotspot(width-caption_width*2,0, width-caption_width,caption_height)) {
            this_e->set.window.fold_state = 2;
            this_e->redraw=1;
            break;
          }
        }
        else if (hotspot(width-caption_width,0, width,caption_height)) {
          this_e->set.window.fold_state = 2;
          this_e->redraw=1;
          break;
        }
      }

      // Title bar drag
      if(hotspot(0,0,width,(this_e->theme->window.height/2)/3)) {
        int i;
        this_e->set.window.state=2; // dragging
        this_e->set.window.offset.x = xmouse-xscreen;
        this_e->set.window.offset.y = ymouse-yscreen;
        this_e->redraw=1;

        // Lower other windows below this one
        for(i=0; i<gui_options.num_elements; i++) {
          if(gui_elements[i].type == GUI_WINDOW) {
            ChangeZDepth(gui_elements[i].owner, 0.9);
          }
        }
        ChangeZDepth("Event Actor", 1.0);
      }
      break;
    }
  }
}


void gui_mouseup() {
  gui* this_e = this_element();
  switch(this_e->type) {
    case GUI_BUTTON:
      this_e->set.button.state=0;
      this_e->redraw=1;
      break;


    case GUI_WINDOW:
    { const int caption_width  = this_e->theme->window_captions.original_width/3;
      const int caption_height = this_e->theme->window_captions.original_height/3;
      if(this_e->set.window.closable && hotspot(width-caption_width,0, width,caption_height)) {
        this_e->set.window.active = 0;
      }
      if(this_e->set.window.foldable) {
        if(this_e->set.window.closable) {
          if(hotspot(width-caption_width*2,0, width-caption_width,caption_height)) {
            this_e->set.window.fold = !this_e->set.window.fold;
          }
        }
        else if (hotspot(width-caption_width,0, width,caption_height)) {
          this_e->set.window.fold = !this_e->set.window.fold;
        }
      }
      this_e->set.window.close_state = 0;
      this_e->set.window.fold_state = 0;
      this_e->set.window.state=0;
      this_e->redraw=1;
      break;
    }
  }
}