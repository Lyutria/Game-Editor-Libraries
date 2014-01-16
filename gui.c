// PRE-REQUISITES:
// > main.c
// > image.c

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

    struct bar { // Displays a bar (like a progress bar)
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

void do_gui(char type[]) {
  if (animpos == 0) {
    gui new_element;
    gui_options.num_elements += 1;
    animpos = gui_options.num_elements;
    new_element.owner_id = animpos;

    if (!strcmp(type, "button")) {
      new_element.type = 1;
    }

    realloc(gui_elements, sizeof(gui)*gui_options.num_elements);
    gui_elements[gui_options.num_elements-1] = new_element;
  } else {
    switch (gui_elements[(int)animpos].type) {
      default:
        erase(0,0,0,0);
        break;

      case 0:
        break;
    }
  }
}
