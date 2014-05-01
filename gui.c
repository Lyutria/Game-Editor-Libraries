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
  Actor* bound_to; // A pointer to the actor that this element is bound to
  int owner_id; // Usually a GUI actor's ANIMPOS variable.
  int type;
  char redraw; // Lets us keep FPS up by only redrawing when we need to
  unsigned char r,g,b; // Colors for the element so you don't have to set it in the Image
  Image* font; // Lets different gui elements have different fonts
  struct gui_theme* theme;

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
      double value; // Value to display
      double last_value;
      int margin; // Number of pixels from (BOTH) sides that the meter starts and ends
                 // (so the edges don't have to be EXACT on the edges of the image for
                 // proper calculations)
      double max; // The max value for the bar/variable
      char text[256];
    } meter;

    struct window { // A window container
      char text[256]; // Window Title
      char buttons[3]; // Up to 3 caption buttons in the top-right
      char active; // If window is shown at all
      char fold; // If window should be folded.
      struct {
        int x;
        int y;
      } offset;
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

gui* this_element() {
  return &gui_elements[(int)animpos-1];
}

void bind_this_element_to(Actor source) {

}

gui* get_element(Actor*source) {
  return &gui_elements[(int)source->animpos-1];
}

void redraw_element(gui* source) {
  source->redraw=1;
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

  // Draw corners
  image_draw(igrid.image[0][0], 0,0);
  image_draw(igrid.image[2][0], width-igrid.column_size,0);
  image_draw(igrid.image[0][2], 0,height-igrid.row_size);
  image_draw(igrid.image[2][2], width-igrid.column_size,height-igrid.row_size);

  // Draw sides
  igrid.image[1][0].width = width-(igrid.column_size*2); //top
  igrid.image[1][2].width = width-(igrid.column_size*2); //bottom
  igrid.image[1][2].fix_canvas = 1;
  igrid.image[0][1].height=height-(igrid.row_size*2); //left
  igrid.image[2][1].height=height-(igrid.row_size*2); //right
  igrid.image[2][1].fix_canvas = 1;
  image_draw(igrid.image[1][0], igrid.column_size,0);
  image_draw(igrid.image[1][2], igrid.column_size,height-igrid.row_size);
  image_draw(igrid.image[0][1], 0,igrid.row_size);
  image_draw(igrid.image[2][1], width-igrid.column_size, igrid.row_size);

  // Draw middle
  igrid.image[1][1].width = width-igrid.column_size*2;
  igrid.image[1][1].height = height-igrid.row_size*2;
  image_draw(igrid.image[1][1], igrid.column_size, igrid.row_size);
  image_freegrid(&igrid);
}

void image_draw_3x3(Image source) {
  image_draw_3x3s(source, width, height);
}


void do_gui(char type[]) {
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

    params = sscanf(type, "%[^:]%*c%[^\n]", buffers[0], buffers[1]);

    if (!strcmp(buffers[0], "button")) {
      new_element.type = GUI_BUTTON;
      if (params>2) {
        sscanf(buffers[1], "%[^\n]", new_element.set.button.text);
      }
    }
    else if (!strcmp(buffers[0], "window")) {
      new_element.type = GUI_WINDOW;
      new_element.set.window.active=1;
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
      sscanf(buffers[1], "%d,%d", &new_element.set.meter.value, &new_element.set.meter.max);
    }

    gui_elements = realloc(gui_elements, sizeof(gui)*gui_options.num_elements);
    gui_elements[gui_options.num_elements-1] = new_element;
  } else {
    gui* this_e = this_element();
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
        int new_state = this_e->set.button.state;
        if(this_e->set.button.state != 2) { // mousedown
          if(!hotspot(0,0,width,height)) {
            new_state = 0; // No mouseover
          } else {
            new_state = 1; // Mouseover
          }
        }
        else {
          if(!hotspot(0,0,width,height)) {
            new_state = 0;
          }
        }

        if(this_e->set.button.state != new_state) {
          this_e->set.button.state = new_state;
          this_e->redraw = 1;
        }

        // PROCESS (binding, etc.)
        // (empty ATM)

        // DRAW
        if(this_e->redraw) {
          int button_height = this_e->theme->button.original_height/3;
          int button_width = this_e->theme->button.original_width;

          Image current_button = image_subimage(this_e->theme->button, 0,button_height*(this_e->set.button.state), button_width,button_height);
          image_setrgb(&current_button, this_e->r, this_e->g, this_e->b);
          erase(0,0,0,1);
          image_draw_3x3(current_button);

          // Draw text
          text_draw_offset(*this_e->font, this_e->set.button.text,
                           (width/2) - ((strlens(this_e->set.button.text)*(this_e->font->char_width))/2) + this_e->theme->text_offset.x,
                           (height/2)- ((this_e->font->height)/2) + this_e->theme->text_offset.y);

          this_e->redraw = 0;
        }
        break;
      }

      // GUI: WINDOWCODE
      // TODO: Add binding, caption buttons
      case GUI_WINDOW:
      {
        // STATES
        int title_height = (this_e->theme->window.height/3)/3;
        if(this_e->set.window.active==1) {
        int new_state = this_e->set.window.state;
          if(this_e->set.window.state != 2) { // dragged
            if(!hotspot(0,0,width,title_height)) {
              new_state = 0; // normal
            } else {
              new_state = 1; // mouseover
            }
          }
          else {
            new_state = 2;
            xscreen = xmouse-this_e->set.window.offset.x;
            yscreen = ymouse-this_e->set.window.offset.y;
          }

          if(this_e->set.window.state != new_state) {
            this_e->set.window.state = new_state;
            this_e->redraw = 1;
          }

          // PROCESS (binding, etc.)
          // (empty ATM)

          // DRAW
          if(this_e->redraw) {
            int window_height = this_e->theme->window.original_height/3;
            int window_width = this_e->theme->window.original_width;
            Image current_window = image_subimage(this_e->theme->window, 0,window_height*(this_e->set.window.state), window_width,window_height);
            image_setrgb(&current_window, this_e->r, this_e->g, this_e->b);
            erase(0,0,0,1);
            image_draw_3x3(current_window);

            // Draw text
            text_draw_offset(*this_e->font, this_e->set.window.text,
                             (width/2) - ((strlens(this_e->set.window.text)*this_e->font->char_width)/2) + this_e->theme->text_offset.x,
                             (title_height/2)- ((this_e->font->height)/2) + this_e->theme->text_offset.y);

            this_e->redraw = 0;
          }
          break;
        } else {
          erase(0,0,0,1);
          break;
        }
      }

      //GUI: LABELCODE
      case GUI_LABEL:
      {
        if(strcmp(text, this_e->set.label.text)) {
          strncpy(this_e->set.label.text, text, 512);
          this_e->redraw=1;
        }

        if(this_e->redraw) {
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
          }
          this_e->redraw=0;
        }
        break;
      }

      //GUI: METERCODE
      //TODO: Figure out why this_e->set.meter.value is set over-max when changed
      case GUI_METER:
      {
        if(this_e->set.meter.value < 0) this_e->set.meter.value = 0;
        else if (this_e->set.meter.value > this_e->set.meter.max) this_e->set.meter.value = this_e->set.meter.max;

        if(this_e->set.meter.last_value != this_e->set.meter.value) {
          this_e->redraw=1;
        }

        if(this_e->redraw) {
          Image*cur = &this_e->theme->meter;
          Image second_layer = image_subimage(*cur,
                                              0,cur->original_height/2,
                                              (double)cur->original_width*((double)this_e->set.meter.value/(double)this_e->set.meter.max),
                                              cur->original_height/2);
          char buffer[64];
          sprintf(buffer, "%d: %d, %d", this_e->set.meter.last_value, this_e->set.meter.value, this_e->set.meter.max);
          sdebug("METER", buffer);
          erase(0,0,0,1);
          if(this_e->set.meter.last_value < this_e->set.meter.value) {
            // Redraw only second layer, it's increasing
            image_draw_3x3s(second_layer, (double)width*(double)(this_e->set.meter.value/this_e->set.meter.max), height);
          } else {
            // Less than, need complete redraw
            Image first_layer = image_subimage(*cur,0,0,cur->original_width,cur->original_height/2);
            image_draw_3x3(first_layer);
            image_draw_3x3s(second_layer, (double)width*(double)(this_e->set.meter.value/this_e->set.meter.max), height);
          }
          this_e->set.meter.last_value = this_e->set.meter.value;
          this_e->redraw=0;
        }
        break;
      }

      //GUI: MENUCODE
      //TODO: visual effects (fading, etc) and binding
      case GUI_MENU:
      {
        // Little need to create redraw code for the menu
        // when we have transp. As always, forced redraw
        // is available if you need it.
        if(this_e->set.menu.active) {
          transp=0;
        } else {
          transp=1;
        }

        if(this_e->redraw) {
          erase(0,0,0,1);
          image_draw_3x3(this_e->theme->menu);
          this_e->redraw=0;
        }
        break;
      }
    }
  }
}

void gui_mousedown() {
  int new_state;
  gui* this_e = this_element();
  switch(this_e->type) {
    case GUI_BUTTON:
      this_e->set.button.state=2;
      this_e->redraw=1;
      break;

    case GUI_WINDOW:
      if(hotspot(0,0,width,(this_e->theme->window.height/2)/3)) {
        this_e->set.window.state=2; // dragging
        mouse_to_actor();
        this_e->set.window.offset.x = xmouse;
        this_e->set.window.offset.y = ymouse;
        this_e->redraw=1;
        mouse_to_screen();
      }
      break;
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
      this_e->set.window.state=0;
      this_e->redraw=1;
      break;
  }
}
