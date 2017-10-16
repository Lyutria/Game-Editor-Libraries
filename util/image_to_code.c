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

void image_to_code(Image source, char gen_name[], char file_name[]) {
  int cur_index=0, index_size=0, x=0, y=0, newline_count=0;
  FILE*  dest_file = fopen(file_name, "w");
  Pixel* index = (Pixel*)calloc(1, sizeof(Pixel));

  if (source.subimage) { debugf("IMG2CODE", "Source is a subimage"); }
  if (!source.data)    { debugf("IMG2CODE", "No data in source"); return; }

  // Code prep
  fprintf(dest_file, "// %s - IMGtoCODE conversion\n// GENERATED CODE FOR AN IMAGE,\n// LOAD INTO GLOBAL CODE\n\n", source.name);
  fprintf(dest_file, "int image_gen_%s(Image* destination) {\n", gen_name);
  fprintf(dest_file, "  Image  i;\n");
  fprintf(dest_file, "  int x, y;\n\n");

  // Generate a color index to reduce code length
  fprintf(dest_file, "  // Color Index:\n");
  for(x=source.topleft.x; x<source.original_width; x++) {
    for(y=source.topleft.y; y<source.original_height; y++){
      int exists = 0;

      for (cur_index=0; cur_index<index_size; cur_index++) {
        if (index[cur_index].r == source.data[x][y].r &&
            index[cur_index].g == source.data[x][y].g &&
            index[cur_index].b == source.data[x][y].b &&
            index[cur_index].t == source.data[x][y].t) {
              exists = 1;
        }
      }

      if (!exists) {
        index_size++;
        index = (Pixel*)realloc(index, (sizeof(Pixel) * index_size));
        index[index_size-1] = source.data[x][y];
      }
    }
  }

  fprintf(dest_file, "  Pixel c[%d] = {\n    ", index_size);
  for (cur_index=0; cur_index<index_size; cur_index++) {
    if (cur_index == index_size-1) {
      fprintf(dest_file, "{%d,%d,%d,%i}\n  };\n\n",
        index[cur_index].r,
        index[cur_index].g,
        index[cur_index].b,
        index[cur_index].t
      );
      break;
    }

    fprintf(dest_file, "{%d,%d,%d,%i},",
      index[cur_index].r,
      index[cur_index].g,
      index[cur_index].b,
      index[cur_index].t
    );

    if (cur_index % 10 == 0 && cur_index != 0) { fprintf(dest_file, "\n    "); }
  }

  fprintf(dest_file, "  if (destination->data != NULL && !destination->subimage) {\n");
  fprintf(dest_file, "    image_free(destination); \n  }\n\n");



  fprintf(dest_file, "  image_new(&i, %d, %d);\n",  source.original_width, source.original_height);
  fprintf(dest_file, "  strcpy(i.name, \"%s\");\n", gen_name);
  fprintf(dest_file, "  i.transparent.r   = %d;\n", source.transparent.r);
  fprintf(dest_file, "  i.transparent.g   = %d;\n", source.transparent.g);
  fprintf(dest_file, "  i.transparent.b   = %d;\n", source.transparent.b);
  fprintf(dest_file, "  i.indexed         = %d;\n", source.indexed);
  fprintf(dest_file, "  i.font            = '%d';\n", source.font);

  // TODO: Save animation information
  // fprintf(dest_file, "  gi.animation       = %d;\n", source.animation)
  // fprintf(dest_file, "  gi.frames          = %d;\n", source.frames)
  // fprintf(dest_file, "  gi.frame_count     = %d;\n", source.frame_count)
  // fprintf(dest_file, "  gi.frame_speed     = %d;\n", source.animation)

  fprintf(dest_file, "  i.characters      = %d;\n",   source.characters);
  fprintf(dest_file, "  i.first_character = '%c';\n", source.first_character);
  fprintf(dest_file, "  i.char_width      = %d;\n", source.char_width);
  fprintf(dest_file, "  i.char_height     = %d;\n", source.char_height);

  // TODO: Save palette information (after palettes are added)

  fprintf(dest_file, "\n  ");
  for(x=source.topleft.x; x<source.original_width; x++) {
    for(y=source.topleft.y; y<source.original_height; y++){
      for (cur_index=0; cur_index<index_size; cur_index++) {
        if (source.data[x][y].r == index[cur_index].r &&
            source.data[x][y].g == index[cur_index].g &&
            source.data[x][y].b == index[cur_index].b &&
            source.data[x][y].t == index[cur_index].t) {
          break;
        }
      }

      fprintf(dest_file, "i.data[%d][%d]=c[%d];", x,y, cur_index);

      if (newline_count++ == 6) {
        fprintf(dest_file, "\n  ");
        newline_count = 0;
      }
    }
  }

  fprintf(dest_file, "\n  *destination = i;");
  fprintf(dest_file, "\n  return 1;\n}");
  free(index);
  fclose(dest_file);
}
