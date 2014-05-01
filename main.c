// THIS FILE IS REQUIRED
// FOR THE OTHER FILES.
// (BECAUSE DEBUG SYSTEM)
//
// EACH FILE IN THIS LIBRARY LISTS
// THE OTHER FILES IT REQUIRES AT
// THE TOP OF THE SOURCE

#ifndef GELIB_MAIN
  #define GELIB_MAIN
#else
  #error This file already exists in global code.
#endif

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

// Generic macros
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
typedef void ARRAY_SIZE;

// Debug options

// Comment out to disable debugging
// Commenting out won't break your code
#define DEBUG_ENABLED

// Comment out if you don't want debug messages also
// going to an error text file, E.g. just using debug
// to display random messages
// The define represents the filename
// #define DEBUG_USE_FILE "debug.txt"

// Number of debug messages stored in memory
#define DEBUG_STACK_SIZE 9

// The max length of debug messages
#define DEBUG_LEN 128


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
//    debug_manage(my_font)
//
// Redraw rates are optimized by the function itself, so you don't
// need to worry about it killing your FPS (unless you debug way too
// often).

#ifdef DEBUG_ENABLED
  struct {
    // Functionality
    // Most set to chars to save memory, stack
    // size should not this value regardless due
    // to performance hits.
    char active;
    char redraw;
    double bg_transparency;
    int file_open;
    // Information
    char buffer[DEBUG_LEN];
    char tbuffer[DEBUG_LEN]; // For use when sprintf'ing a good debug message
    char stack[DEBUG_STACK_SIZE][DEBUG_LEN];
  } DBO = {1, 1, 1, 0};

  #define DEB_STRINGIFY(x) #x
  #define DEB_TOSTRING(x) DEB_STRINGIFY(x)

  #define ndebug(PROMPT, MESG) sprintf(DBO.buffer,strcmp(__FILE__,"STRING")==0 ? "%s: %s" : "%s "__FILE__" "DEB_TOSTRING(__LINE__) ": %s" , PROMPT, MESG);
  #define sdebug(PROMPT, MESG) sprintf(DBO.buffer,strcmp(__FILE__,"STRING")==0 ? "%s: %s" : "%s "__FILE__" "DEB_TOSTRING(__LINE__) ": %s" , PROMPT, MESG); debug_push();
  #define debug(MESG) sdebug("DEBUG", MESG);
  #define assert(EXPR) if(!(EXPR)) {sdebug("ASSERT FAIL");goto error;}
  #define sentinel(MESG) sdebug("SENTINEL", MESG); goto error;
  #define error(MESG) sdebug("ERROR", MESG); goto error;
  #define warn(MESG) sdebug("WARN", MESG);

  // Typedef hacks for GE here. Essentially, these aren't meant to do anything in terms of code.
  // They just trigger false highlights for the above defines, to let you know it's a psuedo-function.
  typedef void ndebug;
  typedef void sdebug;
  typedef void debug;
  typedef void assert;
  typedef void sentinel;
  typedef void error;
  typedef void warn;

  // Pushes the most recent debug statement onto the stack
  void debug_push() {
    int i;
    if(DBO.active) {
      for(i=DEBUG_STACK_SIZE-1; i!=0; i--) {
        strcpy(DBO.stack[i], DBO.stack[i-1]);//, DEBUG_LEN);
      }
      #ifdef DEBUG_USE_FILE
      {
        FILE*debug_filestream;
        if(DBO.file_open) {
          debug_filestream = fopen(DEBUG_USE_FILE, "a");
        }
        else {
          debug_filestream = fopen(DEBUG_USE_FILE, "w");
          DBO.file_open = 1;
        }
        fprintf(debug_filestream, "%s\n", DBO.buffer);
        fclose(debug_filestream);
      }
      #endif

      strcpy(DBO.stack[0], DBO.buffer);//, DEBUG_LEN);
      DBO.redraw = 1;
    }
  }

#else
  #define sdebug(...) ;
  #define debug(...) ;
  #define assert(...) ;
  #define sentinel(...) ;
  #define error(...) ;
  #define warn(...) ;
  void debug_push();
#endif

// (Tools)
// _____           _
//|_   _|__   ___ | |___
//  | |/ _ \ / _ \| / __|
//  | | (_) | (_) | \__ \
//  |_|\___/ \___/|_|___/
//
// Collection of functions for generic usage

int random(int lower, int upper) {
    int v1 = max(upper, lower), v2 = min(upper, lower);
    return (rand(v1-v2)+v2);
}

// Finds the highest clone index out of all clones
// of an actor so you can loop through all the clones
// faster, e.g.:
// //  for(i=0; i<CloneCount(my_actor); i++) {
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

