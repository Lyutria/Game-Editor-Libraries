// C-lib: IMAGE CONVERTER

// PRE-REQUISITES:
// > main.c
// > image.c

// This code will not function in Game-Editor 1.4.1b, due to a file write issue.

// This code allows you to load an image using my functions, then export it
// to a text file as code to load into Game-Editor afterwards. This lets you
// convert an image into pure code which is inside Game-Editor, instead of an
// external file.

#if defined(GELIB_MAIN) && defined(GELIB_IMAGE)
  #ifndef GELIB_IMAGE_TO_CODE
    #define GELIB_IMAGE_TO_CODE
  #else
    #error This file already exists in global code.
  #endif
#else
  #error This code requires MAIN.C, IMAGE.C to be included beforehand.
#endif

// This will create a function that stores a variable color index,
// and manually assign all the pixels through the function.

void image_to_code(Image source, char gen_name[], char file_name[], int mode) {
  FILE* dest_file = fopen(file_name, "w");
  int i, j, k, newline_count=0;
  int index_size;
  Pixel index[1024];

  for(i=0; i<1024; i++) {
    color_index[i][0] = source.transparent.r;
    color_index[i][1] = source.transparent.g;
    color_index[i][2] = source.transparent.b;
  }

  for(i=0; i<source.original_width; i++) {
    for(j=0; j<source.original_height; j++){
    }
  }

  // Code prep
  fprintf(dest_file, "// %s - IMGtoCODE conversion\n// GENERATED CODE FOR AN IMAGE,\n// LOAD INTO GLOBAL CODE\n\n", source.name);
  fprintf(dest_file, "Image image_gen_%s() {\n", gen_name);
  fprintf(dest_file, "  Image gi;\n");
  fprintf(dest_file, "  int i, j;\n");
  fprintf(dest_file, "  // Color Index:\n");

  // Generate a color index to reduce code length
  if(mode == 1) {
    for(i=0; i<index_counter; i++) {
      fprintf(dest_file, "  Pixel c%d = {%d,%d,%d,%f};\n", i, color_index[i][0], color_index[i][1], color_index[i][2]);
    }
  }

  fprintf(dest_file, "  strcpy(gi.name, \"%s\");\n", gen_name);
  fprintf(dest_file, "  gi.width = %d; gi.original_width = gi.width;\n", source.original_width);
  fprintf(dest_file, "  gi.height = %d; gi.original_height = gi.height;\n", source.original_height);
  fprintf(dest_file, "  gi.angle=0;\n");
  fprintf(dest_file, "  gi.r=255;\n");
  fprintf(dest_file, "  gi.g=255;\n");
  fprintf(dest_file, "  gi.b=255;\n");
  fprintf(dest_file, "  gi.scale=1;\n");
  fprintf(dest_file, "  gi.transparent.r=%d;\n", source.transparent.r);
  fprintf(dest_file, "  gi.transparent.g=%d;\n", source.transparent.g);
  fprintf(dest_file, "  gi.transparent.b=%d;\n", source.transparent.b);
  fprintf(dest_file, "  gi.characters=%d;\n", source.characters);
  if(!source.first_character) source.first_character = ' ';
  fprintf(dest_file, "  gi.first_character='%c';\n", source.first_character);
  fprintf(dest_file, "  gi.data = (struct Pixel**)malloc(gi.width * sizeof(struct Pixel*));\n");
  fprintf(dest_file, "  for (i=0; i<gi.width; i++) {\n");
  fprintf(dest_file, "    gi.data[i] = (struct Pixel*)malloc(gi.height * sizeof(struct Pixel));\n  }\n");

  fprintf(dest_file, "  for (i=0; i<gi.width; i++) {\n");
  fprintf(dest_file, "    for (j=0; j<gi.height; j++) {\n");
  fprintf(dest_file, "      gi.data[i][j] = gi.transparent;\n");
  fprintf(dest_file, "    }\n");
  fprintf(dest_file, "  }\n");

  for(i=0; i<source.original_width; i++) {
    for(j=0; j<source.original_height; j++ ){
      if(source.data[i][j].r != source.transparent.r &&
         source.data[i][j].g != source.transparent.g &&
         source.data[i][j].b != source.transparent.b) {
        switch(mode) {
        case 0:
          fprintf(dest_file, "  gi.data[%d][%d].r=%d;", i, j, source.data[i][j].r);
          fprintf(dest_file, "gi.data[%d][%d].g=%d;", i, j, source.data[i][j].g);
          fprintf(dest_file, "gi.data[%d][%d].b=%d;", i, j, source.data[i][j].b);
          newline_count++;
          if(newline_count==3) {
            fprintf(dest_file, "\n");
            newline_count=0;
          }
          break;

        case 1:
          for(k=0; k<index_counter; k++) {
            if(source.data[i][j].r == color_index[k][0] &&
               source.data[i][j].g == color_index[k][1] &&
               source.data[i][j].b == color_index[k][2]) {
              fprintf(dest_file, "gi.data[%d][%d]=c%d;", i, j, k);
              break;
            }
          }

          newline_count++;
          if(newline_count==6) {
            fprintf(dest_file, "\n  ");
            newline_count=0;
          }
          break;

        default:
          break;
        }
      }
    }
  }

  fprintf(dest_file, "\n  return gi;\n}");
  fclose(dest_file);
}
