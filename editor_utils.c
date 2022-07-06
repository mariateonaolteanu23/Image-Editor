#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "editor_utils.h"
#include "matrix_utils.h"
#include "utils.h"

//  checks if there is only one argument in the given string
bool arg_is_one_word(char *args)
{
	//  string is empty
	if (!args)
		return false;

	const char space = ' ';

	//  string contains at least a space
	if (strchr(args, space))
		//  string has more than one argument
		return false;

	//  string contains one argument
	return true;
}

//  checks if the given selection is invalid
bool invalid_selection(my_image *image, int x1, int y1, int x2, int y2)
{
	//  width or height selection is 0
	if (x1 == x2 || y1 == y2)
		return true;

	//  width selection is out of bounds
	if (image->width < x1 || image->width < x2)
		return true;

	//  height selection is out of bounds
	if (image->height < y1 || image->height < y2)
		return true;

	//  selection is valid
	return false;
}

//  checks if a string is not a number
bool not_a_num(char *string)
{
	//  iterate through the string's characters
	while (*string) {
		//  handle negative number
		if (*string == '-' && (*(string + 1) >= '0' || *(string + 1) < '9')) {
			//  get next character
			string++;
			continue;
		}

		//  found a character that is not a number
		if (*string < '0' || *string > '9')
			//  string is not a number
			return true;

		//  get next character
		string++;
	}

	//  string is a number
	return false;
}

//  checks if given string contains the minus sign
bool args_are_negative(char *args)
{
	if (strchr(args, '-'))
		return true;

	return false;
}

//  checks if the given string contains only 4 arguments
bool args_are_4_integers(char *args)
{
	//  there are no arguments
	if (!args)
		return false;

	char *n1, *n2, *n3, *n4;

	//  parse string into 4 strings
	n1 = strtok(args, " ");
	n2 = strtok(NULL, " ");
	n3 = strtok(NULL, " ");
	n4 = strtok(NULL, "\n");

	//  less arguments than required
	if (!n2 || !n3 || !n4)
		return false;

	//  more arguments than required
	if (!arg_is_one_word(n4))
		return false;

	//  one or more of the given arguments are not integers
	if (not_a_num(n1) || not_a_num(n2) || not_a_num(n3) || not_a_num(n4))
		return false;

	//  strings contains 4 arguments
	return true;
}

//  loads an image from a given file
void editor_load(my_image *image, char *args)
{
	//  more than one argument
	if (!arg_is_one_word(args)) {
		printf("Invalid command\n");
		return;
	}

	//  open file
	FILE * file = fopen(args, "r+");

	//  given filename doesn't exist
	if (!file) {
		//  can't load image
		printf("Failed to load %s\n", args);

		//  free previous image
		free_image_data(image);

		//  initialize data for a potential upcoming image
		init_image_data(image);
		return;
	}

	//  there is an image already loaded
	if (!is_empty(image)) {
		//  free previous image's data
		free_image_data(image);
		init_image_data(image);
	}

	//  load new image
	load_image(file, image);

	printf("Loaded %s\n", args);

	//  close file
	fclose(file);
}

//  selects the entire current loaded image
void editor_select_all(my_image *image)
{
	//  no image is loaded
	if (is_empty(image)) {
		printf("No image loaded\n");
		return;
	}

	//  select all image
	set_selection(image->select, 0, 0, image->width, image->height);

	printf("Selected ALL\n");
}

//  selects a section of the current loaded image
void editor_select(my_image *image, char *args)
{
	//  no image is loaded
	if (is_empty(image)) {
		printf("No image loaded\n");
		return;
	}

	//  select command has no arguments
	if (!args) {
		printf("Invalid command\n");
		return;
	}

	//  make copy of given arguments
	char *args_copy = malloc(strlen(args) + 1);
	DIE(!args_copy, "malloc args_copy");

	memcpy(args_copy, args, strlen(args) + 1);

	//  check if given arguments meet the specified conditions
	if (!args_are_4_integers(args)) {
		printf("Invalid command\n");
		free(args_copy);
		return;
	}

	//  check if the given arguments are negative numbers
	if (args_are_negative(args)) {
		printf("Invalid set of coordinates\n");
		free(args_copy);
		return;
	}

	//  arguments are 4 pozitive integers
	//  get selection
	int x1, y1, x2, y2;
	x1 = atoi(strtok(args_copy, " "));
	y1 = atoi(strtok(NULL, " "));
	x2 = atoi(strtok(NULL, " "));
	y2 = atoi(strtok(NULL, "\n"));

	//  free allocated memory for string copy
	free(args_copy);

	//  check if given selection is valid
	if (invalid_selection(image, x1, y1, x2, y2)) {
		printf("Invalid set of coordinates\n");
		return;
	}

	//  store new image selection
	set_selection(image->select, x1, y1, x2, y2);

	printf("Selected %d %d ", image->select->x1, image->select->y1);
	printf("%d %d\n", image->select->x2, image->select->y2);
}

//  checks if the entire image is selected
bool is_selected_all(my_image *image)
{
	//  full height is not selected
	if (image->height != image->select->y2 - image->select->y1)
		return false;

	//  full width is not selected
	if (image->width != image->select->x2 - image->select->x1)
		return false;

	//  entire image is selected
	return true;
}

//  checks if image selection is square
bool selection_is_square(my_image *image)
{
	//  get selection's width
	int w = image->select->x2 - image->select->x1;

	//  get selection's height
	int h = image->select->y2 - image->select->y1;

	//  selection is square
	if (w == h)
		return true;

	//  is not square
	return false;
}

//  checks if an angle value meets the conditions
bool angle_is_unsupported(int ang)
{
	//   value is not supported
	if (ang != 90 && ang != 180 && ang != 270 && ang != 360 && ang != 0)
		return true;

	//   value is supported
	return false;
}

//  rotates the current loaded image
void editor_rotate(my_image *image, char *args)
{
	//  no image is loaded
	if (is_empty(image)) {
		printf("No image loaded\n");
		return;
	}

	//  rotate command has no arguments
	if (!args) {
		printf("Invalid command\n");
		return;
	}

	//  copy the given argument
	char *rotation = malloc(strlen(args) + 1);
	DIE(!rotation, "malloc rotation");

	memcpy(rotation, args, strlen(args) + 1);

	//  get sign
	char sign;

	//  given argument contains "-" sign
	if (args_are_negative(args)) {
		//  set sign
		sign = '-';
		//  point to the start of the number
		args++;
	} else {
		//  sign is plus
		//  set sign
		sign = '+';
	}

	//  get string value of the given angle
	char *str_angle = strtok(args, "\n");

	//  check if string value of angle meets conditions
	//  check if string represents a number
	if (not_a_num(str_angle)) {
		printf("Invalid command\n");
		free(rotation);
		return;
	}

	//  get integer value of the rotation angle
	int angle = atoi(str_angle);

	//  angle is not a specified value
	if (angle_is_unsupported(angle)) {
		printf("Unsupported rotation angle\n");
		free(rotation);
		return;
	}

	//  just a section of the image is selected
	if (!is_selected_all(image)) {
		//  selection is not square
		if (!selection_is_square(image)) {
			printf("The selection must be square\n");
			free(rotation);
			return;
		}

		//  rotate selection of the image
		rotate_image_selection(image, sign, angle);

	} else {
		//  rotate the entire image
		rotate_entire_image(image, sign, angle);
	}

	printf("Rotated %s\n", rotation);

	//  free allocated memory for string
	free(rotation);
}

//  crops the current loaded image
void editor_crop(my_image *image, char *args)
{
	//  no image is loaded
	if (is_empty(image)) {
		printf("No image loaded\n");
		return;
	}

	// crop command has arguments
	if (args) {
		printf("Invalid command\n");
		return;
	}

	//  entire image is selected => no need to crop
	if (is_selected_all(image)) {
		printf("Image cropped\n");
		return;
	}

	// crop image
	crop_image(image);

	printf("Image cropped\n");
}

//  checks if the given filter is invalid
bool apply_filter_is_invalid(char *args)
{
	if (!strncmp(args, "EDGE", sizeof("EDGE") - 1))
		return false;

	if (!strncmp(args, "SHARPEN", sizeof("SHARPEN") - 1))
		return false;

	if (!strncmp(args, "BLUR", sizeof("BLUR") - 1))
		return false;

	if (!strncmp(args, "GAUSSIAN_BLUR", sizeof("GAUSSIAN_BLUR") - 1))
		return false;

	return true;
}

void editor_apply(my_image *image, char *args)
{
	//  no image is loaded
	if (is_empty(image)) {
		printf("No image loaded\n");
		return;
	}

	//  no paramter given
	if (!args) {
		printf("Invalid command\n");
		return;
	}

	//  given filter is invalid
	if (apply_filter_is_invalid(args)) {
		printf("APPLY parameter invalid\n");
		return;
	}

	if (image->img_type == GRAYSCALE) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	//  aplly filter
	apply(image, args);

	printf("APPLY %s done\n", args);
}

//  saves current loaded image to a specified output file
void editor_save(my_image *image, char *args)
{
	//  no image is loaded
	if (is_empty(image)) {
		printf("No image loaded\n");
		return;
	}

	//  get given file name
	char *file_name = strtok(args, " ");

	//  get file formatv(e.g. ascii)
	char *format = strtok(NULL, "\n");

	FILE *output;

	//  output file fotmat is not specified
	if (!format) {
		//  output file is binary
		output = fopen(file_name, "wb+");

		//  save loaded image
		save_image_binary(output, image);

		//  close file
		fclose(output);

		printf("Saved %s\n", args);
		return;
	}

	//  output file is text / ASCII
	output = fopen(file_name, "w+");
	DIE(!output, "fopen output");

	//  save loaded image
	save_image_text(output, image);

	//  close file
	fclose(output);

	printf("Saved %s\n", args);
}

//  frees allocated memory and quits application
void editor_exit(my_image *image)
{
	//  no image is loaded
	if (is_empty(image))
		printf("No image loaded\n");

	//  free pixel matrix and image selection
	free_image_data(image);

	//  free image
	free(image);
	image = NULL;

	//  exit application
	exit(0);
}
