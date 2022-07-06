#ifndef EDITOR_UTTILS_
#define EDITOR_UTTILS_

#include "image_utils.h"

#define MAX_INPUT_LINE_SIZE 100

void editor_load(my_image *image, char *args);

void editor_select_all(my_image *image);

void editor_select(my_image *image, char *args);

void editor_rotate(my_image *image, char *args);

void editor_crop(my_image *image, char *args);

void editor_apply(my_image *image, char *args);

void editor_save(my_image *image, char *args);

void editor_exit(my_image *image);

#endif /* EDITOR_UTTILS_ */
