// PRE-REQUISITES:
// > main.c

// (text structs)
// _____         _
//|_   _|____  _| |_
//  | |/ _ \ \/ / __|
//  | |  __/>  <| |_
//  |_|\___/_/\_\\__|
// ____  _                   _
/// ___|| |_ _ __ _   _  ___| |_ ___
//\___ \| __| '__| | | |/ __| __/ __|
// ___) | |_| |  | |_| | (__| |_\__ \
//|____/ \__|_|   \__,_|\___|\__|___/
//

typedef struct textfile {
  char name[64];
  char** text;
  int lines;
} textfile;

// _____                 _   _
//|  ___|   _ _ __   ___| |_(_) ___  _ __  ___
//| |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//|  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//|_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
//

char chrupper(char source) {
  if (source >= 'a' && source <= 'z') {
    return source - ('a' - 'A');
  }
  return source;
}

char chrlower(char source) {
  if(source >= 'A' && source <= 'Z') {
    return source + ('a' - 'A');
  }
  return source;
}
