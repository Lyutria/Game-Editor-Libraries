// C-lib: TEXT

// PRE-REQUISITES:
// > main.c

#ifndef GELIB_MAIN
  #error This code requires MAIN.C to be included beforehand.
#else
  #ifndef GELIB_TEXT
    #define GELIB_TEXT
  #else
    #error This file already exists in global code.
  #endif
#endif

//  __  ___  __        __  ___  __
// /__`  |  |__) |  | /  `  |  /__`
// .__/  |  |  \ \__/ \__,  |  .__/
//

typedef struct TextFile {
  char name[64];
  char* text;
  int lines;
} TextFile;

//                    __                 ___    __
//  |\/|  /\  |\ | | |__) |  | |     /\   |  | /  \ |\ |
//  |  | /~~\ | \| | |    \__/ |___ /~~\  |  | \__/ | \|
//

// This is used to calculate
int strlens(char source[]) {
  int i, result;
  result = strlen(source);
  for (i=0; i<strlen(source); i++) {
    char c_text = source[i];
    switch (c_text) {
      case '\a':
        result-=3;
        i += 3;
        break;
      case '\r':
        if (source[i+1] == '+' || source[i+1] == '-') { result-=2; i+=2; }
        else { result-=1; i+=1; }
        if (source[i+1] == '+' || source[i+1] == '-') { result-=2; i+=2; }
        else { result-=1; i+=1; }
        break;
      case '\t':
        result-=4;
        break;
    }
  }
  return result;
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

void strninsert(char* dest, int pos, char* in) {
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
  int i; for(i=0; i<strlen(check); i++) { if(source[i] != check[i]) {
      return 0;
    }
  }
  return 1;
}

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

//  ___         ___
// |__  | |    |__
// |    | |___ |___
//

TextFile text_load(char filename[]) {
  TextFile result;
  FILE *file = fopen(filename, "r");
  int cur_line;
  char ch;
  strcpy(result.name, filename);

	if(file == NULL) return result;

  // Goes over the text file twice,
  // this hurts speed, but it's the only
  // way to practically set the malloc
  // size that the dynamic array needs
  // that I know of.

	while((ch = fgetc(file)) != EOF) {
		if(ch == '\n') result.lines++;
  }
	if(ch != '\n') result.lines++;

  fseek(file, 0, SEEK_SET);

  fclose(file);
  return result;
}
