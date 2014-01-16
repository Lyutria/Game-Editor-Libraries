// THIS FILE IS REQUIRED
// FOR THE OTHER FILES.
//
// EACH FILE IN THIS LIBRARY LISTS
// THE OTHER FILES IT REQUIRES AT
// THE TOP OF THE SOURCE

//  ____ _____      _     _ _
// / ___| ____|    | |   (_) |__  ___
//| |  _|  _| _____| |   | | '_ \/ __|
//| |_| | |__|_____| |___| | |_) \__ \
// \____|_____|    |_____|_|_.__/|___/
//
// __  __    _    ___ _   _
//|  \/  |  / \  |_ _| \ | |
//| |\/| | / _ \  | ||  \| |
//| |  | |/ ___ \ | || |\  |
//|_|  |_/_/   \_\___|_| \_|
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

