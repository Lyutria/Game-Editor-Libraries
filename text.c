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

// This is used to calculate visual length for
// our special-format strings used in the library.
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

void strinsert(char* dest, int pos, char* in) {
  char* result = malloc(sizeof(char)*(strlen(dest)+strlen(in)));

  strncpy(result, dest, pos);
  strcat (result, in);
  strcat (result, dest+pos+1);

  strcpy(dest, result);
  free(result);
}

void strninsert(char* dest, int pos, char* in, int max) {
  char* result = malloc(sizeof(char)*(strlen(dest)+strlen(in)));

  strncpy(result, dest, pos);
  strcat (result, in);
  strcat (result, dest+pos+1);

  strncpy(dest, result, max);
  free(result);
}

void chrinsert(char* dest, int pos, char in) {
  int index;
  for (index=strlen(dest)+1; index>pos; index--) {
    dest[index] = dest[index-1];
  }
  dest[pos] = in;
}

void chrremove(char* source, int pos) {
  int index;
  for (index=pos; index<strlen(source); index++) {
    source[index] = source[index+1];
  }
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
