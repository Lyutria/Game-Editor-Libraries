// THIS FILE IS REQUIRED
// FOR THE OTHER FILES.
// (BECAUSE DEBUG SYSTEM)
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

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

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

#define DEBUG_STACK_SIZE 9
#define DEBUG_ENABLED 1

struct {
  char active;
  char redraw;
  char format[32];
} debug_options = {DEBUG_ENABLED, 1, "[%d] %s: %s"};
// Some of the format styles you can try:
// "[%d] %s: %s" (Basic) (default)
// "%d-%s: %s"   (Minimal)
// "[%d:%s] %s"  ("Squared off")
// "[%d:%s]> %s" ("Verbose")
// "%d@%s:$ %s"  ("Bash")

typedef struct debug_state {
  char label[16];
  int priority;
  void (*call)();
} debug_state;

struct debug_part {
  debug_state state;
  char description[64];
} debug_stack[DEBUG_STACK_SIZE];

// 1: Prompt for first half of format
// 2: Priority, if 1 then bypass disabled debug
//    i.e. errors will always be pushed.
// 3: Function with no parameters to call when debug
//    state is used.
debug_state debug_error = {"error", 1, SuspendGame};
debug_state debug_info  = {"info",  0, NULL};
debug_state debug_warn  = {"warn",  0, NULL};

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
  if(debug_options.active || state.priority) {
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

// Finds the highest clone index out of all clones
// of an actor so you can loop through all the clones
// faster, e.g.:
//
//  for(i=0; i<CloneCount(my_actor); i++) {
//    if(my_actor) {
//      // Do stuff
//    }
//  }
int CloneCount(Actor source) {
  int highest_num=0, current_num=0, clones_found=0;
  Actor* current_clone;
  while(clones_found < ActorCount(source.name)) {
    current_clone = getclone2(source.name, current_num);
    if(current_clone) {
      clones_found++;
      if(current_clone->cloneindex > highest_num) {
        highest_num = current_clone->cloneindex;
      }
    }
    current_num++;
  }
  return highest_num;
}




