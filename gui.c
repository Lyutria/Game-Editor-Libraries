// C-lib: GUI

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

// We use the built-in variable of actors in Game-Editor called animpos to
// store the data we use for GUI elements. This way the script can be drop-in,
// and the variable isn't used for Canvas actors.
#define GUI_ID animpos

typedef enum gui_type {
  GUI_OBJECT,
  GUI_BUTTON,
  GUI_SLIDER,
  GUI_METER,
  GUI_WINDOW,
  GUI_LABEL,
  GUI_TOGGLE,
  GUI_COUNTER,
  GUI_TOOLTIP,
  GUI_MENU
} GUIType;

//  __  ___  __        __  ___  __
// /__`  |  |__) |  | /  `  |  /__`
// .__/  |  |  \ \__/ \__,  |  .__/
//

typedef struct gui_theme {
  Image font;
  Image button;
  Image toggle_button;
  Image counter;
  Image textbox;
  Image tooltip;

  Image slider;
  short int slider_offset; // pixels to use as margin for inner slider
  short int slider_handle_width;   // pixel size for handle

  Image meter;
  short int meter_offset; // pixels to use as margin for inner meter

  Image window;
  Image window_highlight;
  Image window_folded;
  Image window_captions;
  short int window_title_offset;

  Image box;
  Image menu;

  struct {
    int x;
    int y;
  } text_offset;
} GUITheme;

typedef struct gui_element {
  char owner[16]; // Owner actor name
  GUIType   type;
  GUITheme* theme;
  Image*    font; // Lets different gui elements have different fonts

  char redraw;
  char initialized;
  unsigned char r,g,b; // Element color modifiers

  char focus;
  char enabled;

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
      char text[64]; // Text to be displayed by said button
      int state; // The state of the button (mouseover, mousedown, etc.)
    } button;


    struct label {
      char text[256];
      char align;
    } label;


    struct toggle_button { // A clickable toggle button
      int* variable; // A pointer to the variable the button needs
      char text[64]; // The text to be displayed by said button
    } toggle_button;


    struct counter { // An increment/decrement button
      char text[64];
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
      int value;
      int last_value;
      int max; // The max value for the bar/variable
      char text[64];
    } meter;


    struct slider { // Displays as a bar with a clickable handle
      int value;
      int last_value;
      int min; // Minimum value for slider
      int max; // Maximum value for slider
      char state; // Drag state
      char show_value; // Whether to show the default value information.
      char text[64]; // Displayed when not moving slider
    } slider;


    struct window { // A window container
      char text[64]; // Window Title
      char buttons[3]; // Up to 3 caption buttons in the top-right
      char text_align;
      char fold; // If window should be folded.
      char foldable;
      char closable;
      char resizable; // default is 0, 1 is true.
      char fold_state;
      char close_state;
      struct {
        int x;
        int y;
      } offset;
      struct {
        short int width;
        short int height;
      } max;
      struct {
        short int width;
        short int height;
      } min;
      char state;
    } window;


    struct menu { // A menu container
      char active; // If menu is being shown
    } menu;


    struct tooltip { // A tooltip, duh
      int delay; // Delay in frames to start fading in
      int active; // If tooltip is active
    } tooltip;
  } set;
} GUIElement;

struct {
  GUIElement* elements; // Dynamic array of GUI elements, to hold them between frames.
  GUITheme    theme;         // Main theme elements default to
  int num_elements;         // Stores the current number of GUI elements
  int mouse_state;
} GUIData;

//  __   __       ___  __   __           ___            __
// /  ` /  \ |\ |  |  |__) /  \ |       |__  |  | |\ | /  `
// \__, \__/ | \|  |  |  \ \__/ |___    |    \__/ | \| \__,
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


GUIElement* this_element() {
  return &GUIData.elements[(int)GUI_ID-1];
}


GUIElement* get_element(Actor*source) {
  if (source->GUI_ID != 0) {
    return &GUIData.elements[(int)source->GUI_ID-1];
  }
  else return NULL;
}


// Bind top, bottom, left, right edges to a source actors width/height;
void bind_this_element_to(Actor*source, char dir[]) {
  GUIElement* this_e = this_element();
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

  if(source->GUI_ID != 0) {
    if(get_element(source)->type == GUI_WINDOW) {
      if(get_element(source)->set.window.fold) { transp = 1; }
      else if(!get_element(source)->enabled) { transp = 1; }
      else { transp = 0; }

      if(this_e->focus) {
        this_e->focus = 0;
        get_element(this_e->bound_to)->focus = 1;
      }
    }
  }

  if(height != old_height || width != old_width || x != old_x || y != old_y) {
    this_e->redraw =1;
  }
}


void redraw_element(Actor* source) {
  get_element(source)->redraw=1;
}


void disable_element(Actor* source) {
  get_element(source)->enabled = 0;
  get_element(source)->redraw  = 1;
}


void enable_element(Actor* source) {
  get_element(source)->enabled = 1;
  get_element(source)->redraw  = 1;
}

// Splits the element into 9 sections and returns
// the location of the mouse.
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

void redraw_all_elements() {
  int i;
  for (i=0; i<GUIData.num_elements; i++) {
    GUIData.elements[i].redraw = 1;
  }
}


void init_gui() {
  GUIData.num_elements = 0;
  GUIData.mouse_state  = 0;
  GUIData.theme.text_offset.x  = 0;
  GUIData.theme.text_offset.y  = 0;
  GUIData.elements = malloc(sizeof(GUIElement));
}

//  __   __                    __
// |  \ |__)  /\  |  | | |\ | / _`
// |__/ |  \ /~~\ |/\| | | \| \__>
//

void gui_draw_label(Image font, char text[], int x_padding, int y, int width, int height, char align) {
  switch(align) {
    case 'l':
      text_draw_offset(font, text, x_padding, (height/2) - (font.height/2) + y);
      break;
    default:
    case 'c':
      text_draw_offset(font, text, (width/2) - ((strlens(text)*font.char_width)/2),
                        (height/2) - (font.height/2) + y);
      break;
    case 'r':
      text_draw_offset(font, text, (width) - (strlens(text)*font.char_width) - x_padding,
                        (height/2) - (font.height/2) + y);
      break;
  }
}

// Splits an image into 9 chunks, stretching the edges and middle
// to fill the specified area with the image. Extremely useful for
// drawing GUI elements to scale.
void image_draw_3x3s(Image source, int x, int y, int width, int height) {
  ImageGrid igrid;
  if (image_gridsplit(source, &igrid, 3, 3) == NULL) { return; }
  // We seperate the grabbed button image into 9 parts for good looking stretching
  // And now draw it

  // Draw middle
  if(width > igrid.column_size*2 && height > igrid.row_size*2) {
    igrid.image[1][1].width  = width-igrid.column_size*2;
    igrid.image[1][1].height = height-igrid.row_size*2;
    image_draw(igrid.image[1][1], x+igrid.column_size, y+igrid.row_size);
  }

  // Draw sides
  igrid.image[1][0].width      = width-(igrid.column_size*2); //top
  igrid.image[1][2].width      = width-(igrid.column_size*2); //bottom
  igrid.image[0][1].height     = height-(igrid.row_size*2); //left
  igrid.image[2][1].height     = height-(igrid.row_size*2); //right
  if(width < igrid.column_size) {
    igrid.image[2][1].width  = width;
    igrid.image[0][1].width  = width;
  }
  else image_draw(igrid.image[2][1], x+width-igrid.column_size, y+igrid.row_size);
  if(height < igrid.row_size) {
    igrid.image[2][1].height = height;
    igrid.image[0][1].height = height;
  }
  else image_draw(igrid.image[1][2], x+igrid.column_size,y+height-igrid.row_size);
  image_draw(igrid.image[0][1], x,igrid.row_size);
  image_draw(igrid.image[1][0], x+igrid.column_size,y);

  // Draw corners
  if(width < igrid.column_size) {
    igrid.image[0][2].width = width;
    igrid.image[0][0].width = width;
    image_draw(igrid.image[0][0], x,y);
    image_draw(igrid.image[0][2], x,y+height-igrid.row_size);
  }
  else if(height < igrid.row_size) {
    igrid.image[2][2].height=height;
    igrid.image[2][0].height=height;
    image_draw(igrid.image[2][0], x+width-igrid.column_size,y);
    image_draw(igrid.image[2][2], x+width-igrid.column_size,y+height-igrid.row_size);
  }
  else {
    image_draw(igrid.image[2][2], x+width-igrid.column_size,y+height-igrid.row_size);
    image_draw(igrid.image[0][2], x,y+height-igrid.row_size);
    image_draw(igrid.image[2][0], x+width-igrid.column_size,y);
    image_draw(igrid.image[0][0], x,y);
  }

  image_freegrid(&igrid);
}


// Draw singular zone from a 3x3, like the function above.
// This is used to draw the window highlights depending on which
// edge your mouse is over.
void image_draw_3x3_section(Image source, int width, int height, int zone) {
  ImageGrid igrid;
  image_gridsplit(source, &igrid, 3, 3);

  // Draw middle
  if(width > igrid.column_size*2 && height > igrid.row_size*2) {
    igrid.image[1][1].width = width-igrid.column_size*2;
    igrid.image[1][1].height = height-igrid.row_size*2;
    if (zone == 5) image_draw(igrid.image[1][1], igrid.column_size, igrid.row_size);
  }

  // Draw sides
  igrid.image[1][0].width = width-(igrid.column_size*2); //top
  igrid.image[1][2].width = width-(igrid.column_size*2); //bottom
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
  image_draw_3x3s(source, 0,0, width, height);
}

//  ___       ___        ___      ___     __   __   __   ___
// |__  |    |__   |\/| |__  |\ |  |     /  ` /  \ |  \ |__
// |___ |___ |___  |  | |___ | \|  |     \__, \__/ |__/ |___
//

int do_create() {
  if(GUI_ID != 0 && this_element()->initialized != 1) {
    this_element()->initialized = 1;
    return 1;
  }
  else return 0;
}


void do_gui_button(GUIElement* this_e) {
  const int button_height = (double)this_e->theme->button.original_height/4.0;
  const int button_width  = this_e->theme->button.original_width;
  int new_state           = this_e->set.button.state;
  int button_mode         = this_e->set.button.state;
  Image current_button;

  if (transp == 1) return;
  if (this_e->set.button.state != 2) { // mousedown
    if   ( hotspot(0,0,width,height) ) { new_state = 1; } // mouseover
    else { new_state = 0; } // normal
  }
  else if(!hotspot(0,0,width,height)) { new_state = 0; }
  if (!this_e->enabled) { new_state = 3; }

  if (this_e->set.button.state != new_state) {
    this_e->set.button.state = new_state;
    this_e->redraw = 1;
  }

  if (!this_e->redraw) return;
  button_mode = new_state;
  image_subimage(this_e->theme->button, &current_button, 0,button_height*(button_mode), button_width,button_height);
  image_setrgb(&current_button, this_e->r, this_e->g, this_e->b);
  erase(0,0,0,1);
  image_draw_3x3(current_button);
  text_draw_offset(*this_e->font, this_e->set.button.text,
                    (width/2) - ((strlens(this_e->set.button.text)*(this_e->font->char_width))/2) + this_e->theme->text_offset.x,
                    (height/2)- ((this_e->font->height)/2) + this_e->theme->text_offset.y);

  this_e->redraw = 0;
}


void do_gui_window(GUIElement* this_e) {
  const int section_size   = (this_e->theme->window.original_height/3)/3;
  const int caption_width  = this_e->theme->window_captions.original_width/3;
  const int caption_height = this_e->theme->window_captions.original_height/3;
  const int window_height  = this_e->theme->window.original_height/3;
  const int window_width   = this_e->theme->window.original_width;
  const int fold_height    = (double)this_e->theme->window_folded.original_height/3.0;
  int fold_state           = this_e->set.window.fold_state;
  int close_state          = this_e->set.window.close_state;
  int new_state            = this_e->set.window.state;
  Image current_window;

  if(!this_e->enabled) { erase(0,0,0,1); return; }
  if (transp == 1) return;

  // If this window has focus, move to front
  if (this_e->focus == 1) {
    int i;
    for(i=0; i<GUIData.num_elements; i++) {
      if(GUIData.elements[i].type == GUI_WINDOW) {
        ChangeZDepth(GUIData.elements[i].owner, 0.9);
      }
    }
    ChangeZDepth("Event Actor", 1.0);

    this_e->focus = 0;
  }

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
    if(width > this_e->set.window.max.width)   { width  = this_e->set.window.max.width; }
    if(height > this_e->set.window.max.height) { height = this_e->set.window.max.height; }
    if(width < this_e->set.window.min.width)   { width  = this_e->set.window.min.width; }
    if(height < this_e->set.window.min.height) { height = this_e->set.window.min.height; }
    this_e->redraw = 1;
  }

  if(this_e->set.window.close_state != close_state) {
    this_e->set.window.close_state   = close_state;
    this_e->redraw                   = 1;
  }
  if(this_e->set.window.fold_state != fold_state) {
    this_e->set.window.fold_state   = fold_state;
    this_e->redraw                  = 1;
  }
  if(this_e->set.window.state != new_state) {
    this_e->set.window.state   = new_state;
    this_e->redraw             = 1;
  }

  if(!this_e->redraw) return;
  if(!this_e->set.window.fold) {
    image_subimage(this_e->theme->window, &current_window,
                   0,window_height*(this_e->set.window.state > 2 ? 0: this_e->set.window.state),
                   window_width,window_height);
    image_setrgb(&current_window, this_e->r, this_e->g, this_e->b);
    erase(0,0,0,1);
    image_draw_3x3(current_window);
  }
  else {
    erase(0,0,0,1);
    image_subimage(this_e->theme->window_folded, &current_window,
                   0,fold_height*(this_e->set.window.state > 2 ? 0: this_e->set.window.state),
                   this_e->theme->window_folded.original_width, fold_height);
    image_draw_3x3s(current_window,0,0,width,fold_height);
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
    Image close_button;
    image_subimage(this_e->theme->window_captions, &close_button,
                   0, close_state*caption_height,
                   caption_width, caption_height);
    image_draw(close_button, width-caption_width,0);
  }
  if(this_e->set.window.foldable) {
    Image fold_button;
    image_subimage(this_e->theme->window_captions, &fold_button,
                   caption_width*(this_e->set.window.fold+1), fold_state*caption_height,
                   caption_width, caption_height);
    if(this_e->set.window.closable) { image_draw(fold_button, width-caption_width*2,0); }
    else                            { image_draw(fold_button, width-caption_width,0); }
  }

  gui_draw_label(this_e->theme->font, this_e->set.window.text,
                 this_e->theme->window_title_offset,0,
                 width, caption_height, this_e->set.window.text_align);

  this_e->redraw = 0;
}


void do_gui_label(GUIElement* this_e) {
  if (transp == 1) return;

  if(strcmp(text, this_e->set.label.text)) {
    strncpy(this_e->set.label.text, text, 256);
    this_e->redraw=1;
  }

  if(!this_e->redraw) return;
  erase(0,0,0,1);
  gui_draw_label(this_e->theme->font, this_e->set.label.text, this_e->theme->text_offset.x, this_e->theme->text_offset.y, width, height, this_e->set.label.align);

  this_e->redraw = 0;
}


void do_gui_meter(GUIElement* this_e) {
  const double b_width = width - this_e->theme->meter_offset*2;
  const int bar_length = (int)(b_width*((double)this_e->set.meter.value/(double)this_e->set.meter.max));
  const Image* cur     = &this_e->theme->meter;
  Image first_layer, second_layer;
  image_subimage(*cur, &first_layer,  0,cur->original_height/2, cur->original_width,cur->original_height/2);
  image_subimage(*cur, &second_layer, 0,0, cur->original_width,cur->original_height/2);

  if (transp == 1) return;

  if      (this_e->set.meter.value < 0) { this_e->set.meter.value = 0; }
  else if (this_e->set.meter.value > this_e->set.meter.max) { this_e->set.meter.value = this_e->set.meter.max; }

  if(this_e->set.meter.last_value != this_e->set.meter.value) {
    this_e->set.meter.last_value = this_e->set.meter.value;
    this_e->redraw=1;
  }

  if(!this_e->redraw) return;
  erase(0,0,0,1);
  image_setrgb(&second_layer, this_e->r, this_e->g, this_e->b);
  image_draw_3x3(first_layer);
  if(bar_length > 0) image_draw_3x3s(second_layer, this_e->theme->meter_offset,0, bar_length, height);
  gui_draw_label(this_e->theme->font, this_e->set.meter.text,
                 this_e->theme->text_offset.x,this_e->theme->text_offset.y,
                 width, height, 'c');
  this_e->redraw=0;
}


void do_gui_slider(GUIElement* this_e) {
  const double b_width = width - this_e->theme->slider_offset*2;
  double b_percent;
  int bar_length, handle_pos;
  char label_text[32];
  const Image* cur = &this_e->theme->slider;
  Image first_layer, second_layer, handle;
  image_subimage(*cur, &first_layer,  0,0,cur->original_width,cur->original_height/3);
  image_subimage(*cur, &second_layer, 0,cur->original_height/3, cur->original_width,cur->original_height/3);
  image_subimage(*cur, &handle,       0,cur->original_height/3*2, cur->original_width,cur->original_height/3);

  if (transp == 1) return;

  if (this_e->set.slider.state == 1) { // drag
    int x_mouse   = xmouse - xscreen - this_e->theme->slider_offset;
    double x_percent;

    if      (x_mouse > b_width) { x_mouse = b_width; }
    else if (x_mouse < 0)       { x_mouse = 0; }

    x_percent = (double)x_mouse / (double)b_width;
    this_e->set.slider.value = ((this_e->set.slider.max-this_e->set.slider.min) * x_percent) + this_e->set.slider.min;
  }

  if      (this_e->set.slider.value < this_e->set.slider.min) { this_e->set.slider.value = this_e->set.slider.min; }
  else if (this_e->set.slider.value > this_e->set.slider.max) { this_e->set.slider.value = this_e->set.slider.max; }

  b_percent  = ((double)(this_e->set.slider.value-this_e->set.slider.min)/(double)(this_e->set.slider.max-this_e->set.slider.min));
  bar_length = b_width * b_percent;

  if(this_e->set.slider.last_value != this_e->set.slider.value) {
    this_e->set.slider.last_value = this_e->set.slider.value;
    this_e->redraw = 1;
  }

  if(!this_e->redraw) return;
  erase(0,0,0,1);
  image_setrgb(&second_layer, this_e->r, this_e->g, this_e->b);
  image_draw_3x3(first_layer);
  if(bar_length > 0) image_draw_3x3s(second_layer, this_e->theme->slider_offset,0, bar_length, height);

  handle_pos = (b_percent * (double)(b_width - this_e->theme->slider_handle_width)) + this_e->theme->slider_offset;
  image_draw_3x3s(handle, handle_pos - handle.original_width/2 + this_e->theme->slider_handle_width/2,0, handle.original_width, height);

  if (this_e->set.slider.show_value) {
    sprintf(label_text, "%d", this_e->set.slider.value);
    gui_draw_label(this_e->theme->font, label_text,
                   this_e->theme->slider_offset + this_e->theme->text_offset.x + 4,
                   this_e->theme->text_offset.y, width, height,
                   (this_e->set.slider.value - this_e->set.slider.min) > (this_e->set.slider.max-this_e->set.slider.min)/2 ? 'l' : 'r');
  }
  this_e->redraw = 0;
}

//  ___       ___        ___      ___                          __   ___  __
// |__  |    |__   |\/| |__  |\ |  |      |\/|  /\  |\ |  /\  / _` |__  |__)
// |___ |___ |___  |  | |___ | \|  |      |  | /~~\ | \| /~~\ \__> |___ |  \
//

void do_gui(char type[]) {
  GUIElement* this_e;

  if (GUI_ID==0) {
    char buffers[2][256];
    int params;
    GUIElement new_element;
    GUIData.num_elements += 1;
    GUI_ID                    = GUIData.num_elements;
    new_element.redraw        = 1;
    new_element.theme         = &GUIData.theme;
    new_element.font          = &GUIData.theme.font;
    new_element.r             = 255;
    new_element.g             = 255;
    new_element.b             = 255;
    new_element.focus         = 0;
    new_element.enabled       = 1;
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
      new_element.enabled = 1;
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
      new_element.set.meter.last_value = -1;
      sscanf(buffers[1], "%i,%i", &new_element.set.meter.value, &new_element.set.meter.max);
    }
    else if (!strcmp(buffers[0], "slider")) {
      new_element.type = GUI_SLIDER;
      new_element.set.slider.last_value = -1;
      new_element.set.slider.state = 0;
      new_element.set.slider.show_value = 1;
      sscanf(buffers[1], "%i,%i,%i", &new_element.set.slider.min, &new_element.set.slider.value, &new_element.set.slider.max);
    }

    GUIData.elements = realloc(GUIData.elements, sizeof(GUIElement)*GUIData.num_elements);
    GUIData.elements[GUIData.num_elements-1] = new_element;
    return;
  }

  this_e = this_element();
  switch(this_e->type) {
    default:
      erase(255,0,0,0);
      break;

    case GUI_OBJECT: break;
    case GUI_BUTTON: do_gui_button(this_e); break;
    case GUI_WINDOW: do_gui_window(this_e); break;
    case GUI_LABEL:  do_gui_label(this_e);  break;
    case GUI_METER:  do_gui_meter(this_e);  break;
    case GUI_SLIDER: do_gui_slider(this_e); break;

    case GUI_MENU:
      if(this_e->set.menu.active) { transp=0; }
      else { transp=1; }

      if(!this_e->redraw) break;
      erase(0,0,0,1);
      image_draw_3x3(this_e->theme->menu);
      this_e->redraw=0;
      break;
  }
}

void gui_mousedown() {
  int new_state;
  GUIElement* this_e = this_element();
  int i;
  for(i=0; i<GUIData.num_elements; i++) {
    GUIData.elements[i].focus = 0;
  }
  this_e->focus = 1;

  switch(this_e->type) {
    case GUI_BUTTON:
    {
      if(!this_e->enabled) break;
      this_e->set.button.state=2;
      this_e->redraw=1;
      break;
    }

    case GUI_SLIDER:
    {
      // I think the expected use of sliders is that the handle should move to wherever
      // you've clicked, rather than just moving the handle.
      // Leaving this code in, in case I end up considering it as an option.

      // double b_width       = width - this_e->theme->slider_offset*2;
      // int handle_width     = this_e->theme->slider.original_width;
      // int h_handle_width   = ceil ((double)this_e->theme->slider_handle_width / 2.0);
      // int bar_length       = b_width * ((double)(this_e->set.slider.value-this_e->set.slider.min)/(double)(this_e->set.slider.max-this_e->set.slider.min));
      // int handle_position  = bar_length + this_e->theme->slider_offset;

      // if (hotspot(handle_position - h_handle_width,0, handle_position + h_handle_width,height)) {
      this_e->set.slider.state = 1;
      this_e->redraw=1;
      // }
    }

    case GUI_WINDOW:
    {
      const int caption_width  = this_e->theme->window_captions.original_width/3;
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
      }
      break;
    }
  }
}


void gui_mouseup() {
  GUIElement* this_e = this_element();
  switch(this_e->type) {
    case GUI_BUTTON:
      this_e->set.button.state=0;
      this_e->redraw=1;
      break;

    case GUI_SLIDER:
      this_e->set.slider.state=0;
      this_e->redraw=1;
      break;

    case GUI_WINDOW:
    { const int caption_width  = this_e->theme->window_captions.original_width/3;
      const int caption_height = this_e->theme->window_captions.original_height/3;
      if(this_e->set.window.closable && hotspot(width-caption_width,0, width,caption_height)) {
        this_e->enabled = 0;
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