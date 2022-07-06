#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "editor_utils.h"
#include "utils.h"

int main(void)
{
	char input_line[MAX_INPUT_LINE_SIZE];
	char *command;
	char *args;
	my_image *image;

	//  alloc image data and initilize it
	image = malloc(sizeof(my_image));
	init_image_data(image);

	do {
		//  get input line
		fgets(input_line, MAX_INPUT_LINE_SIZE, stdin);

		//  get command name
		command = strtok(input_line, " ");

		//  get possible parameter
		args = strtok(NULL, "\n");

		if (!strncmp(command, "LOAD", sizeof("LOAD") - 1)) {
			//  load image
			editor_load(image, args);

		} else if (!strncmp(command, "SELECT", sizeof("SELECT") - 1)) {
			//  select command has no argument
			if (!args) {
				printf("Invalid command\n");
				continue;
			}

			if (!strncmp(args, "ALL", sizeof("ALL") - 1)) {
				//  select all image
				editor_select_all(image);
			} else {
				//  select a section of the image
				editor_select(image, args);
			}
		} else if (!strncmp(command, "ROTATE", sizeof("ROTATE") - 1)) {
			//  rotate image
			editor_rotate(image, args);

		} else if (!strncmp(command, "CROP", sizeof("CROP") - 1)) {
			//  crop image
			editor_crop(image, args);

		} else if (!strncmp(command, "APPLY", sizeof("APPLY") - 1)) {
			//  apply filter on image
			editor_apply(image, args);

		} else if (!strncmp(command, "SAVE", sizeof("SAVE") - 1)) {
			//  save image
			editor_save(image, args);

		} else if (!strncmp(command, "EXIT", sizeof("EXIT") - 1)) {
			//  free resources and exit application
			editor_exit(image);
		} else {
			//  given command is unknown
			printf("Invalid command\n");
		}

	} while (strncmp(command, "EXIT", sizeof("EXIT") - 1) != 0);

	return 0;
}
