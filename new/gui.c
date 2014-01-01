// NAME:     LYU CANVAS API
//
// AUTHOR:   DarkParadox
// VERSION:  0.1 (Alpha)
//
// This API has several parts to it:
//      A canvas-based GUI API with many elements
//      A standard drawing API for the canvas
//      A canvas-based text-drawing API
//
// Using a TGA-loading image system based on structs,
// we can load bitmap fonts just like GE and draw them
// in a canvas instead. Using the same system, we can
// grab an image and cut out parts to use for GUI
// elements, creating an entire canvas GUI system.
//
// Data list:
//
// IMAGE LOADING
// | VARIABLE TYPES | DESCRIPTION |
// | image | holds any image |
//
// | FUNCTIONS | PARAMETERS ||
// | putpixels | int x | x position of pixel to scale |
// ||int y | y position of pixel to scale |
// ||int x_drawscale | how much to stretch width-wise |
// ||int y_drawscale | how much to stretch height-wise |
// ||
// | TGA_Open | char* file_name | path to file |
// ||
// | BMP_Open | char* file_name | path to file |
// ||
// | imageDraw | image image_object | the image to be drawn |
// ||
// | imageFlip | image image_object | the image to be flipped |

typedef struct RGB { // Stores a quick RGBA value
    char r,g,b,a;
} RGB;

typedef struct gui_theme { // Theme struct
    RGB background;
    RGB text;
    RGB border;
    RGB mouseover;
} gui_theme;

struct gui_options {
    gui_theme main_theme; // Main theme elements default to
    int num_elements; // Stores the current number of GUI elements
}; struct gui_options gui_options; // Make a gui_options struct right away.

struct gui {
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


void do_gui(char* type) { // Do-all GUI function.
    if (animpos == 0) { // Check if this actor has had its GUI set up
        gui new_element; // Make a new GUI element
        gui_options.num_elements++; // Increase # of elements
        animpos = gui_options.num_elements; // Assign this elements ID
        new_element.owner_id = animpos;

        if (!strcmp(type, "button")) { // Set up which element we're on
            new_element.type = 1;
        }

        realloc(gui_elements, sizeof(gui)*gui_options.num_elements); // Resize GUI element list.
        gui_elements[guioptions.numelements-1] = new_element; // Assign new element to list
    } else { // If GUI -is- set up
        switch (gui_elements[animpos].type) { // Causes crash if it isn't set up...
            default: // Invalid setup
                erase(0,0,0,0);
                drawText("GUI ERROR: Invalid Typing"); // Warn
                break;

            case 0: // Set up as button
                break;
        }
    }
}
