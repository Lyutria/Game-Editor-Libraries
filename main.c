//            __        __   __                  __
// |  |  /\  |__) |    /  \ /  ` |__/    |    | |__)
// |/\| /~~\ |  \ |___ \__/ \__, |  \    |___ | |__)
//

// This is the base file for the library, must be included first.
// The other files will list what pre-requisites they have at
// the top of their files.

#ifndef GELIB_MAIN
  #define GELIB_MAIN
#else
  #error This file already exists in global code.
#endif


// Game FPS is used to display animations, if you use animations in
// your game, make sure this is set accurately.
#define GAME_FPS 60

// Generic macros
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
typedef void ARRAY_SIZE;

// Comment out to disable debugging
// Commenting out won't break your code
#define DEBUG_ENABLED

// Comment out if you don't want debug messages also
// going to an error text file, E.g. just using debug
// to display random messages
// The define represents the filename
// #define DEBUG_USE_FILE "debug.txt"
// (Doesn't work on Game-Editor 1.4.1 Beta due to file write bug)

//  __   ___  __        __
// |  \ |__  |__) |  | / _`
// |__/ |___ |__) \__/ \__>
//

// To create a debug monitor (prints out debug messages), do this:
//  1. Create a new image in global code
//  2. Set it up as a font.
//  3. Create a new canvas actor
//  4. Set the canvas' draw actor to:
//     debug_manage(my_font)

// Always declare the debugger variables,
// that way if it's disabled, code that uses the debugger variables
// doesn't also have to be removed.

#define DEBUG_STACK_SIZE 9  // # of messages stored, minimum 1
#define DEBUG_PROMPT_LEN 16
#define DEBUG_LEN 128       // message length.

typedef struct {
  Actor* source;
  char prompt[DEBUG_PROMPT_LEN];
  char text[DEBUG_LEN];
} DebugMessage;

struct {
  // Functionality
  // Most set to chars to save memory, stack
  // size should not this value regardless due
  // to performance hits.
  char  active;
  char  redraw;
  float  bg_transparency;
  char  file_open;
  FILE* stream;

  // Information
  char buffer[DEBUG_LEN]; // For use when sprintf'ing a good debug message
  DebugMessage stack[DEBUG_STACK_SIZE];
  DebugMessage message;
} debugger = {1, 1, 1, 0};

#ifdef DEBUG_ENABLED
  #define DEB_STRINGIFY(x) #x
  #define DEB_TOSTRING(x) DEB_STRINGIFY(x)

  // Pushes the most recent debug statement onto the stack
  int debug_push() {
    int i;
    if(debugger.active) {
      for(i=DEBUG_STACK_SIZE-1; i!=0; i--) {
        debugger.stack[i] = debugger.stack[i-1];
      }
      // Decides whether to write failed assertions and
      // other details to files.
      #ifdef DEBUG_USE_FILE
      {
        if(!debugger.file_open) {
          debugger.stream = fopen(DEBUG_USE_FILE, "w+");
          debugger.file_open= 1;
        }
        fprintf(debugger.stream, "%s\n", debugger.message.text);
      }
      #endif

      debugger.stack[0] = debugger.message;
      debugger.redraw = 1;
    }
    return 1;
  }

  void debugf(char prompt[], char message[]) {
    char name_buffer[64];
    sprintf(name_buffer, "%s.0", name);
    debugger.message.source = getclone(name_buffer);
    strncpy(debugger.message.prompt, prompt, DEBUG_PROMPT_LEN);
    strncpy(debugger.message.text, message, DEBUG_LEN);
    debug_push();
  }

  void debug(char message[]) {
    debugf("DEBUG", message);
  }

#else
  void debug_push();
  void debugf(char a[], char b[]);
  void debug(char a[]);
#endif

// ___  __   __        __
//  |  /  \ /  \ |    /__`
//  |  \__/ \__/ |___ .__/
//

int random(int lower, int upper) {
    int v1 = max(upper, lower), v2 = min(upper, lower);
    return (rand(v1-v2)+v2);
}

// returns x% of 2nd param
// returns -1 on fail
double percent_of(double percent, double source) {
  if(percent <= 0 || percent > 100 || source == 0) {
    return -1;
  }
  percent = percent/100.0;
  return (source*percent);
}

// returns a number signifying what percent of the 2nd param the first is
// E.g. returns "50" if params are (5, 10);
double what_percent_of(double fp, double sp) {
  double result;
  if(fp == 0 || sp == 0) {
    return -1;
  }
  result = fp/sp;
  result = result*100.0;
  return result;
}

// converts a CHAR to an INT
// returns -1 if invalid char
int ctoi(char ch) {
  if(ch >= '0' && ch <='9') {
    return (int)(ch-'0');
  }
  return 0;
}

