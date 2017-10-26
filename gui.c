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

typedef enum GUIType {
  GUI_OBJECT,
  GUI_BUTTON,
  GUI_SLIDER,
  GUI_METER,
  GUI_WINDOW,
  GUI_LABEL,
  GUI_TEXTBOX,
  GUI_TOGGLE,
  GUI_COUNTER,
  GUI_TOOLTIP,
  GUI_MENU
} GUIType;

//  __  ___  __        __  ___  __
// /__`  |  |__) |  | /  `  |  /__`
// .__/  |  |  \ \__/ \__,  |  .__/
//

typedef struct GUITheme {
  Image font;
  Image button;
  Image toggle_button;
  Image counter;
  Image textbox;
  Image tooltip;

  Image slider;
  short int slider_offset; // pixels to use as margin for inner slider
  short int slider_handle_width; // pixel size for handle

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

typedef struct GUIElement {
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

  union gui_properties { // A union to save memory space, since
              // There's a lot of variables.
    struct button_properties { // A clickable button
      char text[64]; // Text to be displayed by said button
      int state; // The state of the button (mouseover, mousedown, etc.)
    } button;


    struct label_properties {
      char text[256];
      char align;
    } label;


    struct toggle_button_properties { // A clickable toggle button
      int* variable; // A pointer to the variable the button needs
      char text[64]; // The text to be displayed by said button
    } toggle_button;


    struct counter_properties { // An increment/decrement button
      char text[64];
      int* variable;
      char left_delimiter; // The text for the decrementer.
      char right_delimiter; // Text for the incrementer
    } counter;


    struct textbox_properties { // A single-line textbox
      char* text;
      int   max_size;
      int   last_max;
      int   shift;
      int  select_start;
      char  state;
      struct {
        int pos;
        short int speed;
        short int timer;
      } caret;
      char limit_input; // if active, only allow characters from allowed_symbols
      char allowed_symbols[256];
    } textbox;


    struct meter_properties { // Displays a bar (like a progress bar)
      int value;
      int last_value;
      int max; // The max value for the bar/variable
      char text[64];
    } meter;


    struct slider_properties { // Displays as a bar with a clickable handle
      int value;
      int last_value;
      int min; // Minimum value for slider
      int max; // Maximum value for slider
      char state; // Drag state
      char show_value; // Whether to show the default value information.
      char text[64]; // Displayed when not moving slider
    } slider;


    struct window_properties { // A window container
      char text[64]; // Window Title
      char buttons[3]; // Up to 3 caption buttons in the top-right
      char text_align;
      char fold; // If window should be folded.
      char foldable;
      char closable;
      char resizable;
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


    struct menu_properties { // A menu container
      char active; // If menu is being shown
    } menu;


    struct tooltip_properties { // A tooltip, duh
      int delay; // Delay in frames to start fading in
      int active; // If tooltip is active
    } tooltip;
  } set;
} GUIElement;

struct {
  GUIElement* elements; // Dynamic array of GUI elements, to hold them between frames.
  GUITheme    theme;    // Main theme elements default to
  int num_elements;     // Stores the current number of GUI elements
  char clipboard[512];  // Stores program clipboard for text elements
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
  if (GUI_ID == 0) { return NULL; }
  return &GUIData.elements[(int)GUI_ID-1];
}


GUIElement* get_element(Actor*source) {
  if (source->GUI_ID == 0) { return NULL; }
  return &GUIData.elements[(int)source->GUI_ID-1];
}


// Bind top, bottom, left, right edges to a source actors width/height;
void bind_this_element_to(Actor*source, char dir[]) {
  GUIElement* elem = this_element();
  int old_x = x, old_y = y;
  int old_width = width, old_height = height;
  if ( elem->bound_to != source ) { // setup
    int i;
    ChangeParent("Event Actor", source->name);
    for(i=0; i<strlen(dir); i++) {
      switch(dir[i]) {
        case 'b':
        case 'd':
          elem->bound.d = 1;
          elem->bound.d_distance = source->height - height - y;
          break;
        case 't':
        case 'u':
          elem->bound.u = 1;
          elem->bound.u_distance = y;
          break;
        case 'l':
          elem->bound.l = 1;
          elem->bound.l_distance = x;
          break;
        case 'r':
          elem->bound.r_distance = source->width - width - x;
          elem->bound.r = 1;
          break;
        }
      }
      elem->bound_to = source;
  }

  if (elem->bound.l && elem->bound.r) {
    x = elem->bound.l_distance;
    width = elem->bound_to->width - elem->bound.l_distance - elem->bound.r_distance;
  }
  else if (elem->bound.l) {
    x = elem->bound.l_distance;
  }
  else if (elem->bound.r) {
    x = elem->bound_to->width - elem->bound.r_distance - width;
  }

  if (elem->bound.u && elem->bound.d) {
    y = elem->bound.u_distance;
    height = elem->bound_to->height - elem->bound.u_distance - elem->bound.d_distance;
  }
  else if (elem->bound.u) {
    y = elem->bound.u_distance;
  }
  else if (elem->bound.d) {
    y = elem->bound_to->height - elem->bound.d_distance - height;
  }

  if(source->GUI_ID != 0) {
    if(get_element(source)->type == GUI_WINDOW) {
      if(get_element(source)->set.window.fold) { transp = 1; }
      else if(!get_element(source)->enabled) { transp = 1; }
      else { transp = 0; }

      if(elem->focus) {
        get_element(elem->bound_to)->focus = 1;
      }
    }
  }

  if(height != old_height || width != old_width || x != old_x || y != old_y) {
    elem->redraw =1;
  }
}


void redraw_element(Actor* source) {
  if (!get_element(source)) { return; }
  get_element(source)->redraw=1;
}


void disable_element(Actor* source) {
  if (!get_element(source)) { return; }
  get_element(source)->enabled = 0;
  get_element(source)->redraw  = 1;
}


void enable_element(Actor* source) {
  if (!get_element(source)) { return; }
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


void gui_button() {
  GUIElement* elem = this_element();
  const int button_height = (double)elem->theme->button.source_height/4.0;
  const int button_width  = elem->theme->button.source_width;
  int new_state           = elem->set.button.state;
  int button_mode         = elem->set.button.state;
  Image current_button;

  if (transp == 1) return;
  if (elem->set.button.state != 2) { // mousedown
    if   ( hotspot(0,0,width,height) ) { new_state = 1; } // mouseover
    else { new_state = 0; } // normal
  }
  else if(!hotspot(0,0,width,height)) { new_state = 0; }
  if (!elem->enabled) { new_state = 3; }

  if (elem->set.button.state != new_state) {
    elem->set.button.state = new_state;
    elem->redraw = 1;
  }

  if (!elem->redraw) return;
  button_mode = new_state;
  image_subimage(elem->theme->button, &current_button, 0,button_height*(button_mode), button_width,button_height);
  image_setrgb(&current_button, elem->r, elem->g, elem->b);
  erase(0,0,0,1);
  image_draw_3x3(current_button);
  text_draw_offset(elem->theme->font, elem->set.button.text,
                   (width/2) - ((strlens(elem->set.button.text)*(elem->theme->font.char_width))/2) + elem->theme->text_offset.x,
                   (height/2)- ((elem->theme->font.height)/2) + elem->theme->text_offset.y);

  elem->redraw = 0;
}


void gui_window() {
  GUIElement* elem = this_element();
  const int section_size   = (elem->theme->window.source_height/3)/3;
  const int caption_width  = elem->theme->window_captions.source_width/3;
  const int caption_height = elem->theme->window_captions.source_height/3;
  const int window_height  = elem->theme->window.source_height/3;
  const int window_width   = elem->theme->window.source_width;
  const int fold_height    = (double)elem->theme->window_folded.source_height/3.0;
  int fold_state           = elem->set.window.fold_state;
  int close_state          = elem->set.window.close_state;
  int new_state            = elem->set.window.state;
  Image current_window;

  if(!elem->enabled) { erase(0,0,0,1); return; }
  if (transp == 1) return;

  // If this window has focus, move to front
  if (elem->focus == 1) {
    int i;
    for(i=0; i<GUIData.num_elements; i++) {
      if(GUIData.elements[i].type == GUI_WINDOW) {
        ChangeZDepth(GUIData.elements[i].owner, 0.9);
      }
    }
    ChangeZDepth("Event Actor", 1.0);

    elem->focus = 0;
  }

  if(elem->set.window.closable && hotspot(width-caption_width,0, width,caption_height)) {
    if(elem->set.window.close_state != 2) { close_state = 1; }
  }
  else if (close_state != 2) { close_state = 0; }

  if(elem->set.window.foldable) {
    if (fold_state != 2) { fold_state = 0; }
    if(elem->set.window.closable) {
      if(hotspot(width-caption_width*2,0, width-caption_width,caption_height)) {
        if(elem->set.window.fold_state != 2) { fold_state = 1; }
      }
    }
    else if (hotspot(width-caption_width,0, width,caption_height)) {
      if(elem->set.window.fold_state != 2) { fold_state = 1; }
    }
  }

  if(elem->set.window.state != 2 && elem->set.window.state < 4) { // dragged
    if(hotspot(0,0,width,height) && !hotspot(3,3,width-3,height-3) && elem->set.window.resizable) {
      new_state = 3; // edge highlighted
    } else if(hotspot(0,0,width,section_size)) {
      new_state = 1; // mouseover
      if (fold_state || close_state) new_state = 0;
    } else {
      new_state = 0; // normal
    }
  }
  else if (new_state == 2) { // dragging
    xscreen = xmouse-elem->set.window.offset.x;
    yscreen = ymouse-elem->set.window.offset.y;
  }
  else if (new_state >= 4) { // resizing
    const int x_mouse = xmouse - xscreen;
    const int y_mouse = ymouse - yscreen;
    const int state = new_state-3;
    if (state == 1 || state == 2 || state == 3) {
      if (height-y_mouse > elem->set.window.min.height) {
        if (height < elem->set.window.max.height) { y += y_mouse; }
        height = height - y_mouse;
      }
    }
    if (state == 1 || state == 4 || state == 7) {
      if (width-x_mouse > elem->set.window.min.width) {
        if (width < elem->set.window.max.width) { x += x_mouse; }
        width = width - x_mouse;
      }
    }
    if (state == 3 || state == 6 || state == 9) { width = x_mouse; }
    if (state == 7 || state == 8 || state == 9) { height = y_mouse; }
    if(width > elem->set.window.max.width)   { width  = elem->set.window.max.width; }
    if(height > elem->set.window.max.height) { height = elem->set.window.max.height; }
    if(width < elem->set.window.min.width)   { width  = elem->set.window.min.width; }
    if(height < elem->set.window.min.height) { height = elem->set.window.min.height; }
    elem->redraw = 1;
  }

  if(elem->set.window.close_state != close_state) {
    elem->set.window.close_state   = close_state;
    elem->redraw                   = 1;
  }
  if(elem->set.window.fold_state != fold_state) {
    elem->set.window.fold_state   = fold_state;
    elem->redraw                  = 1;
  }
  if(elem->set.window.state != new_state) {
    elem->set.window.state   = new_state;
    elem->redraw             = 1;
  }

  if(!elem->redraw) return;
  if(!elem->set.window.fold) {
    image_subimage(elem->theme->window, &current_window,
                   0,window_height*(elem->set.window.state > 2 ? 0: elem->set.window.state),
                   window_width,window_height);
    image_setrgb(&current_window, elem->r, elem->g, elem->b);
    erase(0,0,0,1);
    image_draw_3x3(current_window);
  }
  else {
    erase(0,0,0,1);
    image_subimage(elem->theme->window_folded, &current_window,
                   0,fold_height*(elem->set.window.state > 2 ? 0: elem->set.window.state),
                   elem->theme->window_folded.source_width, fold_height);
    image_draw_3x3s(current_window,0,0,width,fold_height);
  }

  // Edge highlights for resizing
  if(new_state >= 3) {
    int zone = gui_find_section(window_height/3);
    if (!elem->set.window.fold) {
      image_draw_3x3_section(elem->theme->window_highlight, width,height, zone);
    }
  }

  // Draw caption buttons
  if(elem->set.window.closable) {
    Image close_button;
    image_subimage(elem->theme->window_captions, &close_button,
                   0, close_state*caption_height,
                   caption_width, caption_height);
    image_draw(close_button, width-caption_width,0);
  }
  if(elem->set.window.foldable) {
    Image fold_button;
    image_subimage(elem->theme->window_captions, &fold_button,
                   caption_width*(elem->set.window.fold+1), fold_state*caption_height,
                   caption_width, caption_height);
    if(elem->set.window.closable) { image_draw(fold_button, width-caption_width*2,0); }
    else                            { image_draw(fold_button, width-caption_width,0); }
  }

  gui_draw_label(elem->theme->font, elem->set.window.text,
                 elem->theme->window_title_offset,0,
                 width, caption_height, elem->set.window.text_align);

  elem->redraw = 0;
}


void gui_label() {
  GUIElement* elem = this_element();
  if (transp == 1) return;

  if(strcmp(text, elem->set.label.text)) {
    strncpy(elem->set.label.text, text, 256);
    elem->redraw=1;
  }

  if(!elem->redraw) return;
  erase(0,0,0,1);
  gui_draw_label(elem->theme->font, elem->set.label.text, elem->theme->text_offset.x, elem->theme->text_offset.y, width, height, elem->set.label.align);

  elem->redraw = 0;
}


void gui_meter() {
  GUIElement* elem = this_element();
  const double b_width = width - elem->theme->meter_offset*2;
  const int bar_length = (int)(b_width*((double)elem->set.meter.value/(double)elem->set.meter.max));
  const Image* cur     = &elem->theme->meter;
  Image first_layer, second_layer;

  if (transp == 1) return;

  if      (elem->set.meter.value < 0) { elem->set.meter.value = 0; }
  else if (elem->set.meter.value > elem->set.meter.max) { elem->set.meter.value = elem->set.meter.max; }

  if(elem->set.meter.last_value != elem->set.meter.value) {
    elem->set.meter.last_value = elem->set.meter.value;
    elem->redraw=1;
  }

  if(!elem->redraw) return;
  image_subimage(*cur, &first_layer,  0,cur->source_height/2, cur->source_width,cur->source_height/2);
  image_subimage(*cur, &second_layer, 0,0, cur->source_width,cur->source_height/2);
  erase(0,0,0,1);
  image_setrgb(&second_layer, elem->r, elem->g, elem->b);
  image_draw_3x3(first_layer);
  if(bar_length > 0) image_draw_3x3s(second_layer, elem->theme->meter_offset,0, bar_length, height);
  gui_draw_label(elem->theme->font, elem->set.meter.text,
                 elem->theme->text_offset.x,elem->theme->text_offset.y,
                 width, height, 'c');
  elem->redraw=0;
}


void gui_slider() {
  GUIElement* elem = this_element();
  const double b_width = width - elem->theme->slider_offset*2;
  double b_percent;
  int bar_length, handle_pos;
  char label_text[32];
  const Image* cur = &elem->theme->slider;
  Image first_layer, second_layer, handle;

  if (transp == 1) return;

  if (elem->set.slider.state == 1) { // drag
    int x_mouse   = xmouse - xscreen - elem->theme->slider_offset;
    double x_percent;

    if      (x_mouse > b_width) { x_mouse = b_width; }
    else if (x_mouse < 0)       { x_mouse = 0; }

    x_percent = (double)x_mouse / (double)b_width;
    elem->set.slider.value = ((elem->set.slider.max-elem->set.slider.min) * x_percent) + elem->set.slider.min;
  }

  if      (elem->set.slider.value < elem->set.slider.min) { elem->set.slider.value = elem->set.slider.min; }
  else if (elem->set.slider.value > elem->set.slider.max) { elem->set.slider.value = elem->set.slider.max; }

  b_percent  = ((double)(elem->set.slider.value-elem->set.slider.min)/(double)(elem->set.slider.max-elem->set.slider.min));
  bar_length = b_width * b_percent;

  if(elem->set.slider.last_value != elem->set.slider.value) {
    elem->set.slider.last_value = elem->set.slider.value;
    elem->redraw = 1;
  }

  if(!elem->redraw) return;
  image_subimage(*cur, &first_layer,  0,0,cur->source_width,cur->source_height/3);
  image_subimage(*cur, &second_layer, 0,cur->source_height/3, cur->source_width,cur->source_height/3);
  image_subimage(*cur, &handle,       0,cur->source_height/3*2, cur->source_width,cur->source_height/3);
  erase(0,0,0,1);
  image_setrgb(&second_layer, elem->r, elem->g, elem->b);
  image_draw_3x3(first_layer);
  if(bar_length > 0) image_draw_3x3s(second_layer, elem->theme->slider_offset,0, bar_length, height);

  handle_pos = (b_percent * (double)(b_width - elem->theme->slider_handle_width)) + elem->theme->slider_offset;
  image_draw_3x3s(handle, handle_pos,0, handle.source_width, height);

  if (elem->set.slider.show_value) {
    sprintf(label_text, "%d", elem->set.slider.value);
    gui_draw_label(elem->theme->font, label_text,
                   elem->theme->slider_offset + elem->theme->text_offset.x + 4,
                   elem->theme->text_offset.y, width, height,
                   (elem->set.slider.value - elem->set.slider.min) > (elem->set.slider.max-elem->set.slider.min)/2 ? 'l' : 'r');
  }
  elem->redraw = 0;
}

void gui_textbox() {
  GUIElement* elem = this_element();
  const Image* cur        = &elem->theme->textbox;
  const int    x_padding  = cur->source_width/3;
  const int    x_mouse    = min(max(xmouse - xscreen, 0), width);
  int          car_pos    = elem->theme->font.char_width*elem->set.textbox.caret.pos + x_padding;
  int          select_init, select_size;
  Image base, caret, select;

  if (transp == 1) { return; }

  if (elem->set.textbox.state == 1)  { // dragging
    elem->set.textbox.caret.pos = min(strlen(elem->set.textbox.text) * elem->theme->font.char_width,
                                       ((float)(elem->set.textbox.shift*-1) + x_mouse - x_padding + elem->theme->font.char_width/2 ) / (float)elem->theme->font.char_width);
    if (elem->set.textbox.caret.pos < 0) { elem->set.textbox.caret.pos = 0; }
    if (elem->set.textbox.caret.pos > strlen(elem->set.textbox.text)) { elem->set.textbox.caret.pos = strlen(elem->set.textbox.text); }
  }

  if (car_pos + elem->set.textbox.shift  > width - x_padding && width - car_pos - x_padding < elem->set.textbox.shift) {
    elem->set.textbox.shift = (width - car_pos) - x_padding;
  }
  else if (car_pos + elem->set.textbox.shift < x_padding) {
    elem->set.textbox.shift = (0 - car_pos) + x_padding;
  }

  if (elem->focus) {
    elem->set.textbox.caret.timer += 1;
    if (elem->set.textbox.caret.timer == elem->set.textbox.caret.speed)   { elem->redraw = 1; }
    if (elem->set.textbox.caret.timer == elem->set.textbox.caret.speed*2) {
      elem->redraw = 1;
      elem->set.textbox.caret.timer = 0;
    }
  }
  else { elem->set.textbox.select_start = -1; }

  if (elem->set.textbox.caret.pos > strlen(elem->set.textbox.text)) { elem->set.textbox.caret.pos = strlen(elem->set.textbox.text); }

  select_init = min(elem->set.textbox.select_start, elem->set.textbox.caret.pos);
  select_size = max(elem->set.textbox.select_start, elem->set.textbox.caret.pos) - select_init;

  if(!elem->redraw) return;
  image_subimage(*cur, &base,  0,cur->source_height/4*elem->focus,cur->source_width,cur->source_height/4);
  image_subimage(*cur, &caret, 0,cur->source_height/4*2, cur->source_width, cur->source_height/4);
  image_subimage(*cur, &select,0,cur->source_height/4*3, cur->source_width, cur->source_height/4);
  erase(0,0,0,1);
  image_draw_3x3(base);
  if (elem->set.textbox.select_start != -1 && elem->set.textbox.caret.pos - elem->set.textbox.select_start != 0) {
    image_draw_3x3s(select, ((select_init+1)*elem->theme->font.char_width) + elem->set.textbox.shift - 2,
                    height/2 - elem->theme->font.char_height/2,
                    select_size*elem->theme->font.char_width + 2,
                    elem->theme->font.char_height);
  }
  text_draw_offset(elem->theme->font, elem->set.textbox.text,
    x_padding + elem->set.textbox.shift, height/2 - elem->theme->font.height/2);
  if (elem->focus && elem->set.textbox.caret.timer < elem->set.textbox.caret.speed) {
    image_draw(caret, car_pos + elem->set.textbox.shift, height/2 - caret.source_height/2);
  }
}

//  ___       ___        ___      ___                          __   ___  __
// |__  |    |__   |\/| |__  |\ |  |      |\/|  /\  |\ |  /\  / _` |__  |__)
// |___ |___ |___  |  | |___ | \|  |      |  | /~~\ | \| /~~\ \__> |___ |  \
//

void gui_create(GUIType type, char properties[]);
void gui_draw();

void do_gui(char type[]) {
  GUIElement* elem;

  if (GUI_ID==0) {
    char buffers[2][256];
    int params;
    GUIElement new_element;
    GUIData.num_elements += 1;
    GUI_ID                    = GUIData.num_elements;
    new_element.redraw        = 1;
    new_element.theme         = &GUIData.theme;
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
      new_element.set.slider.state      = 0;
      new_element.set.slider.show_value = 1;
      sscanf(buffers[1], "%i,%i,%i", &new_element.set.slider.min, &new_element.set.slider.value, &new_element.set.slider.max);
    }
    else if (!strcmp(buffers[0], "textbox")) {
      new_element.type = GUI_TEXTBOX;
      new_element.set.textbox.max_size     = 128;
      new_element.set.textbox.last_max     = new_element.set.textbox.max_size;
      new_element.set.textbox.caret.speed  = GAME_FPS/3;
      new_element.set.textbox.select_start = -1;
      new_element.set.textbox.text     = malloc(sizeof(char)*new_element.set.textbox.max_size);
    }

    GUIData.elements = realloc(GUIData.elements, sizeof(GUIElement)*GUIData.num_elements);
    GUIData.elements[GUIData.num_elements-1] = new_element;
    return;
  }

  elem = this_element();
  switch(elem->type) {
    default:
      erase(255,0,0,0);
      break;

    case GUI_OBJECT: break;
    case GUI_BUTTON:  gui_button();  break;
    case GUI_WINDOW:  gui_window();  break;
    case GUI_LABEL:   gui_label();   break;
    case GUI_METER:   gui_meter();   break;
    case GUI_SLIDER:  gui_slider();  break;
    case GUI_TEXTBOX: gui_textbox(); break;

    case GUI_MENU: {
      if(elem->set.menu.active) { transp=0; }
      else { transp=1; }

      if(!elem->redraw) break;
      erase(0,0,0,1);
      image_draw_3x3(elem->theme->menu);
      elem->redraw=0;
    } break;
  }
}

void gui_mousedown() {
  GUIElement* elem = this_element();
  int i;
  for(i=0; i<GUIData.num_elements; i++) {
    GUIData.elements[i].focus = 0;
  }
  elem->focus = 1;

  switch(elem->type) {
    case GUI_BUTTON: {
      if(!elem->enabled) break;
      elem->set.button.state=2;
      elem->redraw=1;
    } break;

    case GUI_SLIDER: {
      elem->set.slider.state = 1;
      elem->redraw=1;
    } break;

    case GUI_WINDOW: {
      const int caption_width  = elem->theme->window_captions.source_width/3;
      const int caption_height = elem->theme->window_captions.source_height/3;
      // Edge resizing
      if(hotspot(0,0,width,height) && !hotspot(3,3,width-3,height-3) && elem->set.window.resizable) {
        int zone = gui_find_section(elem->theme->window_highlight.source_height/3);
        if (elem->set.window.fold) {
          if(zone == 1 || zone == 4 || zone == 7) {
            elem->set.window.state = 3 + 4;
            elem->redraw = 1;
          }
          else if (zone == 3 || zone == 6 || zone == 9) {
            elem->set.window.state = 3 + 6;
            elem->redraw = 1;
          }
          break;
        }
        elem->set.window.state = 3 + zone;
        elem->redraw=1;
        break;
      }

      // Caption buttons
      if(elem->set.window.closable && hotspot(width-caption_width,0, width,caption_height)) {
        elem->set.window.close_state = 2;
        elem->redraw=1;
        break;
      }
      if(elem->set.window.foldable) {
        if(elem->set.window.closable) {
          if(hotspot(width-caption_width*2,0, width-caption_width,caption_height)) {
            elem->set.window.fold_state = 2;
            elem->redraw=1;
            break;
          }
        }
        else if (hotspot(width-caption_width,0, width,caption_height)) {
          elem->set.window.fold_state = 2;
          elem->redraw=1;
          break;
        }
      }

      // Title bar drag
      if(hotspot(0,0,width,(elem->theme->window.height/2)/3)) {
        int i;
        elem->set.window.state=2; // dragging
        elem->set.window.offset.x = xmouse-xscreen;
        elem->set.window.offset.y = ymouse-yscreen;
        elem->redraw=1;
      }
    } break;

    case GUI_TEXTBOX: {
      int new_pos;
      const int x_padding = elem->theme->textbox.source_width/3;
      const int x_mouse   = min(max(xmouse - xscreen, 0), width);
      elem->set.textbox.caret.pos = ((float)(elem->set.textbox.shift*-1) + x_mouse - x_padding + elem->theme->font.char_width/2 ) / (float)elem->theme->font.char_width;
      elem->set.textbox.caret.pos = min(max(elem->set.textbox.caret.pos, 0), strlen(elem->set.textbox.text));
      elem->set.textbox.select_start = elem->set.textbox.caret.pos;
      elem->set.textbox.state = 1;
    } break;
  }
}


void gui_mouseup() {
  GUIElement* elem = this_element();
  switch(elem->type) {
    case GUI_BUTTON: {
      elem->set.button.state=0;
      elem->redraw=1;
    } break;

    case GUI_SLIDER: {
      elem->set.slider.state=0;
      elem->redraw=1;
    } break;

    case GUI_WINDOW: {
      const int caption_width  = elem->theme->window_captions.source_width/3;
      const int caption_height = elem->theme->window_captions.source_height/3;
      if(elem->set.window.closable && hotspot(width-caption_width,0, width,caption_height)) {
        elem->enabled = 0;
      }
      if(elem->set.window.foldable) {
        if(elem->set.window.closable) {
          if(hotspot(width-caption_width*2,0, width-caption_width,caption_height)) {
            elem->set.window.fold = !elem->set.window.fold;
          }
        }
        else if (hotspot(width-caption_width,0, width,caption_height)) {
          elem->set.window.fold = !elem->set.window.fold;
        }
      }
      elem->set.window.close_state = 0;
      elem->set.window.fold_state = 0;
      elem->set.window.state=0;
      elem->redraw=1;
    } break;

    case GUI_TEXTBOX: {
      if (elem->set.textbox.select_start == elem->set.textbox.caret.pos) {
        elem->set.textbox.select_start = -1;
      }
      elem->set.textbox.state = 0;
    } break;
  }
}

void gui_keydown() {
  GUIElement* elem = this_element();
  switch(elem->type) {
    case GUI_TEXTBOX: {
      // This is used because Game-Editor uses ANSI C, so we can't
      // even use a const int to declare array size, but I'd like
      // to be able to make sure all the arrays are allocated to the
      // same size.
      #define VIS_KEYS 64
      char* key      = GetKeyState();
      int   last_key = getLastKey();
      char* dest     = elem->set.textbox.text;
      int   pos, is_visible=0;

      // Store a key -> result dictionary for any keys that produce visible
      // letters or symbols.
      char visible_keys[VIS_KEYS] = {
        KEY_a, KEY_b, KEY_c, KEY_d, KEY_e, KEY_f, KEY_g, KEY_h, KEY_i, KEY_j, KEY_k, KEY_l,
        KEY_m, KEY_n, KEY_o, KEY_p, KEY_q, KEY_r, KEY_s, KEY_t, KEY_u, KEY_v, KEY_w, KEY_x,
        KEY_y, KEY_z, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
        KEY_BACKQUOTE, KEY_MINUS, KEY_EQUALS, KEY_LEFTBRACKET, KEY_RIGHTBRACKET, KEY_BACKSLASH,
        KEY_SEMICOLON, KEY_QUOTE, KEY_COMMA,  KEY_PERIOD,      KEY_SLASH,        KEY_SPACE,
        KEY_PAD_0, KEY_PAD_1, KEY_PAD_2, KEY_PAD_3, KEY_PAD_4, KEY_PAD_5, KEY_PAD_6, KEY_PAD_7,
        KEY_PAD_8, KEY_PAD_9, KEY_PAD_PERIOD, KEY_PAD_DIVIDE, KEY_PAD_MULTIPLY, KEY_PAD_MINUS,
        KEY_PAD_PLUS, KEY_PAD_EQUALS
      };
      char keys_lowercase[VIS_KEYS] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
        'y', 'z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
        '`', '-', '=', '[', ']','\\',
        ';','\'', ',', '.', '/', ' ',
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', '.', '/', '*', '-',
        '+', '='

      };
      char keys_uppercase[VIS_KEYS] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
        'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
        '~', '_', '+', '{', '}', '|',
        ':', '"', '<', '>', '?', ' ',
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', '.', '/', '*', '-',
        '+', '='
      };

      if (!elem->focus) { break; }

      // This resets the caret blink timer while typing, so that the caret
      // doesn't go away as long as you're still pressing keys.
      elem->set.textbox.caret.timer = 0;

      // Any halting command characters
      // The main reason these are in IF statements instead of
      // a switch case is that we want to be able to break out of
      // the overarching switch case early.
      if (last_key == KEY_LEFT) {
        if      (!key[KEY_LSHIFT] && !key[KEY_RSHIFT])    { elem->set.textbox.select_start = -1; }
        else if (elem->set.textbox.select_start == -1) { elem->set.textbox.select_start = elem->set.textbox.caret.pos; }
        elem->set.textbox.caret.pos = max(elem->set.textbox.caret.pos-1, 0);
        break;
      }

      if (last_key == KEY_RIGHT) {
        if      (!key[KEY_LSHIFT] && !key[KEY_RSHIFT])    { elem->set.textbox.select_start = -1; }
        else if (elem->set.textbox.select_start == -1) { elem->set.textbox.select_start = elem->set.textbox.caret.pos; }
        elem->set.textbox.caret.pos = min(elem->set.textbox.caret.pos+1, strlen(elem->set.textbox.text));
        break;
      }

      if (last_key == KEY_END) {
        if      (!key[KEY_LSHIFT] && !key[KEY_RSHIFT])    { elem->set.textbox.select_start = -1; }
        else if (elem->set.textbox.select_start == -1) { elem->set.textbox.select_start = elem->set.textbox.caret.pos; }
        elem->set.textbox.caret.pos = strlen(elem->set.textbox.text);
        break;
      }

      if (last_key == KEY_HOME) {
        if      (!key[KEY_LSHIFT] && !key[KEY_RSHIFT])    { elem->set.textbox.select_start = -1; }
        else if (elem->set.textbox.select_start == -1) { elem->set.textbox.select_start = elem->set.textbox.caret.pos; }
        elem->set.textbox.caret.pos = 0;
        break;
      }

      if (last_key == KEY_BACKSPACE) {
        if (strlen(dest) == 0) { break; }

        if (elem->set.textbox.select_start != -1) {
          int init, len, i;
          init = min(elem->set.textbox.caret.pos, elem->set.textbox.select_start);
          len  = max(elem->set.textbox.caret.pos, elem->set.textbox.select_start) - init;

          for (i=init; i<strlen(elem->set.textbox.text); i++) {
            char res;
            if (i + len > strlen(elem->set.textbox.text)) { res = '\0'; }
            else { res = elem->set.textbox.text[i + len]; }
            elem->set.textbox.text[i] = elem->set.textbox.text[i + len];
          }

          elem->set.textbox.caret.pos    = init;
          elem->set.textbox.shift       += len * elem->theme->font.char_width;
          if (elem->set.textbox.shift > 0) { elem->set.textbox.shift = 0; }
          elem->set.textbox.select_start = -1;
          break;
        }
        else {
          if (elem->set.textbox.caret.pos == 0) { break; }
          chrremove(dest, elem->set.textbox.caret.pos-1);
          elem->set.textbox.caret.pos--;

          if (elem->set.textbox.shift < 0) {
            elem->set.textbox.shift += elem->theme->font.char_width;
            elem->set.textbox.shift  = min(elem->set.textbox.shift, 0);
          }
        }
        break;
      }

      if (last_key == KEY_RETURN ||
          last_key == KEY_PAD_ENTER) {
        elem->set.textbox.select_start = -1;
        break;
      }

      if ( last_key == KEY_ESCAPE ) {
        elem->set.textbox.select_start = -1;
        elem->focus = 0; break;
      }

      // Modifier keys
      if (key[KEY_LCTRL] || key[KEY_RCTRL]) {
        if (last_key == KEY_a) {
          elem->set.textbox.select_start = 0;
          elem->set.textbox.caret.pos    = strlen(elem->set.textbox.text);
        }
        break;
      }

      for (pos=0; pos<VIS_KEYS; pos++) {
        if (visible_keys[pos] == last_key) { is_visible = 1; break; }
      }

      if (is_visible) {
        char* dict;
        if   (key[KEY_LSHIFT] || key[KEY_RSHIFT]) { dict = keys_uppercase; }
        else dict = keys_lowercase;


        if (elem->set.textbox.select_start != -1) {
          int init, len, i;
          init = min(elem->set.textbox.caret.pos, elem->set.textbox.select_start);
          len  = max(elem->set.textbox.caret.pos, elem->set.textbox.select_start) - init;

          for (i=init; i<strlen(elem->set.textbox.text); i++) {
            char res;
            if (i + len > strlen(elem->set.textbox.text)) { res = '\0'; }
            else { res = elem->set.textbox.text[i + len]; }
            elem->set.textbox.text[i] = elem->set.textbox.text[i + len];
          }

          elem->set.textbox.caret.pos    = init;
          elem->set.textbox.select_start = -1;
        }

        if (strlen(elem->set.textbox.text) > elem->set.textbox.max_size) { break; }
        chrinsert(dest, elem->set.textbox.caret.pos, dict[pos]);
        elem->set.textbox.caret.pos++;
      }

      #undef VIS_KEYS
    } break;
  }
}