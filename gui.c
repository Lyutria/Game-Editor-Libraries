// PRE-REQUISITES:
// > main.c
// > image.c
// > text.c

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
} gui_options; // Make a gui_options struct right away.

struct gui_theme {
  Image font;
  Image button;
  Image label;
  Image toggle_button;
  Image counter;
  Image textbox;
  Image button_toggle;
  Image tooltip;
  Image slider;
  Image meter;
  Image window;
  Image menu;
  struct {
    int x;
    int y;
  } text_offset;
} gui_theme;

typedef struct gui {
  int owner_id; // Usually a GUI actor's ANIMPOS variable.
  int type;
  Actor* bound_to; // A pointer to the actor that this element is bound to
  int state; // Lets the drawer know what state to draw in
  char redraw; // Lets us keep FPS up by only redrawing when we need to
  struct gui_theme* theme;
  Image *font; // Lets different gui elements have different fonts

  union elements { // A union to save memory space, since
           // There's a lot of variables.
    struct button { // A clickable button
      char text[256]; // Text to be displayed by said button
    } button;

    struct label {
      char text[256];
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
      double *variable; // Pointer to variable to display
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
  mouse_to_actor();
  if(xmouse>x1 && ymouse>y1 && xmouse<x2 && ymouse<y2) {
    mouse_to_screen();
    return 1;
  }
  else {
    mouse_to_screen();
    return 0;
  }
}

gui* get_this_element() {
  return &gui_elements[(int)animpos-1];
}

void init_gui() {
  gui_options.num_elements=0;
  gui_theme.text_offset.x=0;
  gui_theme.text_offset.y=0;
  gui_elements = malloc(sizeof(gui));
}


void do_gui(char type[]) {
  if (animpos == 0) {
    char buffers[2][256];
    int params;
    gui new_element;
    gui_options.num_elements += 1;
    animpos = gui_options.num_elements;
    new_element.owner_id = animpos;
    new_element.state = 0;
    new_element.redraw = 1;
    new_element.theme = &gui_theme;

    params = sscanf(type, "%[^:]%*c%[^\n]", buffers[0], buffers[1]);

    if (!strcmp(buffers[0], "button")) {
      new_element.type = GUI_BUTTON;
      if (params>2) {
        sscanf(buffers[1], "%[^\n]", new_element.elements.button.text);
      }
    }

    gui_elements = realloc(gui_elements, sizeof(gui)*gui_options.num_elements);
    gui_elements[gui_options.num_elements-1] = new_element;
  } else {
    gui* this_e = get_this_element();
    switch(this_e->type) {
      default:
      {
        erase(255,0,0,0);
        break;
      }

      // GUI: BUTTON CODE
      case GUI_BUTTON:
      {
        // STATES
        int new_state = this_e->state;
        if(this_e->state != 2) { // mousedown
          if(!hotspot(0,0,width,height)) {
            new_state = 0; // No mouseover
          } else {
            new_state = 1; // Mouseover
          }
        }
        else {
          if(!hotspot(0,0,width,height)) {
            new_state = 0;
          } else {
            this_e->redraw = 1;
          }
        }

        if(this_e->state != new_state) {
          this_e->state = new_state;
          this_e->redraw = 1;
        }

        // PROCESS (binding, etc.)
        // (empty ATM)

        // DRAW
        if(this_e->redraw) {
          int button_height = this_e->theme->button.original_height/3;
          int button_width = this_e->theme->button.original_width;
          int char_width  = this_e->theme->font.original_width / this_e->theme->font.characters;
          Image current_button = image_subimage(this_e->theme->button, 0,button_height*(this_e->state), button_width,button_height);
          ImageGrid bt = image_gridsplit(current_button, 3, 3);
          // We seperate the grabbed button image into 9 parts for good looking stretching
          // And now draw it
          erase(0,0,0,1);

          // Draw corners
          image_draw(bt.image[0][0], 0,0);
          image_draw(bt.image[2][0], width-bt.column_size,0);
          image_draw(bt.image[0][2], 0,height-bt.row_size);
          image_draw(bt.image[2][2], width-bt.column_size,height-bt.row_size);

          // Draw sides
          bt.image[1][0].width = width-(bt.column_size*2); //top
          bt.image[1][2].width = width-(bt.column_size*2); //bottom
          bt.image[1][2].fix_canvas = 1;
          bt.image[0][1].height=height-(bt.row_size*2); //left
          bt.image[2][1].height=height-(bt.row_size*2); //right
          bt.image[2][1].fix_canvas = 1;
          image_draw(bt.image[1][0], bt.column_size,0);
          image_draw(bt.image[1][2], bt.column_size,height-bt.row_size);
          image_draw(bt.image[0][1], 0,bt.row_size);
          image_draw(bt.image[2][1], width-bt.column_size, bt.row_size);

          // Draw middle
          bt.image[1][1].width = width-bt.column_size*2;
          bt.image[1][1].height = height-bt.row_size*2;
          image_draw(bt.image[1][1], bt.column_size, bt.row_size);

          // Draw text
          text_draw_offset(this_e->theme->font, this_e->elements.button.text,
                           (width/2) - ((strlens(this_e->elements.button.text)*char_width)/2) + this_e->theme->text_offset.x,
                           (height/2)- ((this_e->theme->font.height)/2) + this_e->theme->text_offset.y);

          image_freegrid(&bt);
          this_e->redraw = 0;
        }
        break;
      }
    }
  }
}

void gui_mousedown() {
  switch(get_this_element()->type) {
    case GUI_BUTTON:
      get_this_element()->state=2;
      break;
  }
}

void gui_mouseup() {
  switch(get_this_element()->type) {
    case GUI_BUTTON:
      get_this_element()->state=0;
      break;
  }
}
