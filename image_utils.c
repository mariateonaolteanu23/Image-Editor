#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "image_utils.h"
#include "matrix_utils.h"
#include "utils.h"

//  no image is loaded
bool is_empty(my_image *image)
{
	return !image->img;
}

//  ignores the commented lines in given file
void handle_comments(FILE *file, char *input_line)
{
	long cursor = ftell(file);

	while (fgets(input_line, MAX_LINE_SIZE, file)) {
		if (input_line[0] != '#') {
			fseek(file, cursor, SEEK_SET);
			break;
		}

	   cursor = ftell(file);
	}

	fgets(input_line, MAX_LINE_SIZE, file);
}

//  swaps 2 integers
void swap_int(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

//  sets an image's selection
void set_selection(my_select *select, int x1, int y1, int x2, int y2)
{
	//  handle possible inverted order
	if (x1 > x2)
		swap_int(&x1, &x2);

	if (y1 > y2)
		swap_int(&y1, &y2);

	select->x1 = x1;
	select->x2 = x2;
	select->y1 = y1;
	select->y2 = y2;
}

//  intializes image's data
void init_image_data(my_image *image)
{
	image->height = 0;
	image->width = 0;
	image->pixel_value = 0;

	image->img = NULL;
	image->select = malloc(sizeof(my_select));
}

//  frees image's data (slection & pixel matrix)
void free_image_data(my_image *image)
{
	if (image->select) {
		//  free selection
		free(image->select);
		image->select = NULL;
	}

	//  free pixel matrix / matrices
	if (!is_empty(image)) {
		if (image->img_type == COLOR) {
			//  get color image
			color_img *color = (color_img *)image->img;

			//  free image's color channels
			free_matrix(color->red, image->height);
			free_matrix(color->green, image->height);
			free_matrix(color->blue, image->height);
		} else {
			//  get basic image
			basic_img *basic = (basic_img *)image->img;

			//  free image's pixel matrix
			free_matrix(basic->pixels, image->height);
		}

		free(image->img);
		image->img = NULL;
	}
}

//  sets the type (e.g grayscale) and the file type of the given image
void set_image_attributes(my_image *image, int number)
{
	switch (number) {
	case 1:
			image->file_type = TEXT;
			image->img_type = BLACK_WHITE;
			return;
	case 2:
			image->file_type = TEXT;
			image->img_type = GRAYSCALE;
			return;
	case 3:
			image->file_type = TEXT;
			image->img_type = COLOR;
			return;
	case 4:
			image->file_type = BINARY;
			image->img_type = BLACK_WHITE;
			return;
	case 5:
			image->file_type = BINARY;
			image->img_type = GRAYSCALE;
			return;
	case 6:
			image->file_type = BINARY;
			image->img_type = COLOR;
			return;
	}
}

//  stores pixels data
void set_pixel_matrix(my_image *image, void *data, int data_size)
{
	image->img = malloc(data_size);
	DIE(!image->img, "malloc image->img");

	memcpy(image->img, data, data_size);
}

//  loads color image's pixel matrix from given file
void load_color_image(FILE *file, my_image *image)
{
	color_img color;
	int height, width;

	//  get dimesnions
	height = image->height;
	width = image->width;

	//   alloc memory for the image's color channels
	color.red = alloc_matrix(height, width);
	color.green = alloc_matrix(height, width);
	color.blue = alloc_matrix(height, width);

	//  load pixel matrices from binary file
	if (image->file_type == BINARY) {
		b_3_load(file, color.red, color.green, color.blue, height, width);
	} else {
		//  load pixel matrices from text file
		t_3_load(file, color.red, color.green, color.blue, height, width);
	}

	//  store pixel matrix
	set_pixel_matrix(image, &color, sizeof(color_img));
}

//  loads basic image's pixel matrix from given file
void load_basic_image(FILE *file, my_image *image)
{
	basic_img basic;
	int height, width;

	//  get dimesnions
	height = image->height;
	width = image->width;

	basic.pixels = alloc_matrix(height, width);

	//  load pixel matrix from binary file
	if (image->file_type == BINARY) {
		b_load(file, basic.pixels, height, width);
	} else {
		//  load pixel matrices from text file
		t_load(file, basic.pixels, height, width);
	}

	//  store pixel matrix
	set_pixel_matrix(image, &basic, sizeof(basic_img));
}

//  loads image's data from given file
void load_image(FILE *file, my_image *image)
{
	char input_line[MAX_LINE_SIZE];
	char *p;

	//  ignore possible comments
	handle_comments(file, input_line);

	//  get magic number
	p = strtok(input_line, "\n");
	set_image_attributes(image, atoi(&p[1]));

	///  ignore possible comments
	handle_comments(file, input_line);

	//  set image dimensions
	image->width = atoi(strtok(input_line, " "));
	image->height = atoi(strtok(NULL, "\n"));

	//  set initial image selection (selects all)
	set_selection(image->select, 0, 0, image->width, image->height);

	//  set pixel max value
	if (image->img_type == BLACK_WHITE) {
		image->pixel_value = 1;
	} else {
		//  ignore possible comments
		handle_comments(file, input_line);
		image->pixel_value = atoi(strtok(input_line, "\n"));
	}

	//  ignore possible comments
	handle_comments(file, input_line);

	//  move to the start of the pixel matrix
	long cursor = ftell(file);
	fseek(file, cursor - strlen(input_line), SEEK_SET);

	//  load pixel matrix
	if (image->img_type == COLOR) {
		load_color_image(file, image);
		return;
	}

	load_basic_image(file, image);
}

//  rotates inplace a square section of the given color image
void rotate_color_image_selection(my_image *image, char sign, int angle)
{
	int x1, x2, y1;

	//  get color image
	color_img *color = (color_img *)image->img;

	//  get selection
	x1 = image->select->x1;
	y1 = image->select->y1;
	x2 = image->select->x2;

	//  rotate 180 degrees clockwise
	if ((sign == '-' && angle == 180) || (sign == '+' && angle == 180)) {
		//  rotate image's color channels
		rotate_180_inplace(color->red, x1, y1, x2 - x1);
		rotate_180_inplace(color->green, x1, y1, x2 - x1);
		rotate_180_inplace(color->blue, x1, y1, x2 - x1);
		return;
	}

	//  rotate 270 degrees clockwise
	if ((sign == '-' && angle == 90) || (sign == '+' && angle == 270)) {
		//  rotate image's color channels
		rotate_270_inplace(color->red, x1, y1, x2 - x1);
		rotate_270_inplace(color->green, x1, y1, x2 - x1);
		rotate_270_inplace(color->blue, x1, y1, x2 - x1);
		return;
	}

	//  rotate 90 degrees clockwise
	if ((sign == '+' && angle == 90) || (sign == '-' && angle == 270)) {
		//  rotate image's color channels
		rotate_90_inplace(color->red, x1, y1, x2 - x1);
		rotate_90_inplace(color->green, x1, y1, x2 - x1);
		rotate_90_inplace(color->blue, x1, y1, x2 - x1);
		return;
	}
}

//  rotates inplace a square section of the given basic image
void rotate_basic_image_selection(my_image *image, char sign, int angle)
{
	int x1, x2, y1, y2;

	//  get basic image
	basic_img *basic = (basic_img *)image->img;

	//  get selection
	x1 = image->select->x1;
	y1 = image->select->y1;
	x2 = image->select->x2;
	y2 = image->select->y2;

	if ((sign == '-' && angle == 180) || (sign == '+' && angle == 180)) {
		//  rotate image
		rotate_180_inplace(basic->pixels, x1, y1, x2 - x1);
		return;
	}

	//  rotate 270 degrees clockwise
	if ((sign == '-' && angle == 90) || (sign == '+' && angle == 270)) {
		rotate_270_inplace(basic->pixels, x1, y1, x2 - x1);

		//  update picture selection
		set_selection(image->select, y1, x1, y2, x2);
		return;
	}

	//  rotate 90 degrees clockwise
	if ((sign == '+' && angle == 90) || (sign == '-' && angle == 270)) {
		rotate_90_inplace(basic->pixels, x1, y1, x2 - x1);

		//  update picture
		set_selection(image->select, y1, x1, y2, x2);
		return;
	}
}

//  rotates inplace a square section of the loaded image
void rotate_image_selection(my_image *image, char sign, int angle)
{
	//  rotate selection of color image
	if (image->img_type == COLOR) {
		rotate_color_image_selection(image, sign, angle);
		return;
	}

	//  rotate selection of basic image
	rotate_basic_image_selection(image, sign, angle);
}

//  rotates a full basic image
void rotate_entire_basic_image(my_image *image, char sign, int angle)
{
	double **rotate;
	int new_height, new_width;

	//  no neeed to rotate
	if (angle == 0)
		return;

	//  no neeed to rotate
	if ((sign == '-' && angle == 360) || (sign == '+' && angle == 360))
		return;

	//  get basic image
	basic_img *basic = (basic_img *)image->img;

	//  rotate 180 degrees clockwise
	if ((sign == '-' && angle == 180) || (sign == '+' && angle == 180)) {
		//  rotate image
		rotate = rotate_180(basic->pixels, image->height, image->width);

		//  update dimensions
		new_height = image->height;
		new_width = image->width;
	}

	//  rotate 270 degrees clockwise
	if ((sign == '-' && angle == 90) || (sign == '+' && angle == 270)) {
		//  rotate image
		rotate = rotate_270(basic->pixels, image->height, image->width);

		//  update dimensions
		new_height = image->width;
		new_width = image->height;
	}

	//  rotate 90 degrees clockwise
	if ((sign == '+' && angle == 90) || (sign == '-' && angle == 270)) {
		//  rotate image
		rotate = rotate_90(basic->pixels, image->height, image->width);

		//  update dimensions
		new_height = image->width;
		new_width = image->height;
	}

	//  free the previous image
	free_matrix(basic->pixels, image->height);

	//  store the rotated image
	basic->pixels = rotate;

	//  set image's updated dimmensions
	image->height = new_height;
	image->width = new_width;

	//  set image's updated selection
	set_selection(image->select, 0, 0, new_width, new_height);
}

//  rotates a full basic image
void rotate_entire_color_image(my_image *image, char sign, int angle)
{
	double **rotate_r;
	double **rotate_g;
	double **rotate_b;
	int new_height, new_width;

	//  no neeed to rotate
	if (angle == 0)
		return;

	//  no neeed to rotate
	if ((sign == '-' && angle == 360) || (sign == '+' && angle == 360))
		return;

	//  get color image
	color_img *color = (color_img *)image->img;

	//  rotate 180 degrees clockwise
	if ((sign == '-' && angle == 180) || (sign == '+' && angle == 180)) {
		//  rotate image's color channels
		rotate_r = rotate_180(color->red, image->height, image->width);
		rotate_g = rotate_180(color->green, image->height, image->width);
		rotate_b = rotate_180(color->blue, image->height, image->width);

		//  update dimensions
		new_height = image->height;
		new_width = image->width;
	}

	//  rotate 270 degrees clockwise
	if ((sign == '-' && angle == 90) || (sign == '+' && angle == 270)) {
		//  rotate image's color channels
		rotate_r = rotate_270(color->red, image->height, image->width);
		rotate_g = rotate_270(color->green, image->height, image->width);
		rotate_b = rotate_270(color->blue, image->height, image->width);

		//  update dimensions
		new_height = image->width;
		new_width = image->height;
	}

	//  rotate 90 degrees clockwise
	if ((sign == '+' && angle == 90) || (sign == '-' && angle == 270)) {
		//  rotate image's color channels
		rotate_r = rotate_90(color->red, image->height, image->width);
		rotate_g = rotate_90(color->green, image->height, image->width);
		rotate_b = rotate_90(color->blue, image->height, image->width);

		//  update dimensions
		new_height = image->width;
		new_width = image->height;
	}

	//  free previos color channels
	free_matrix(color->red, image->height);
	free_matrix(color->green, image->height);
	free_matrix(color->blue, image->height);

	//  store rotated image's channels
	color->red = rotate_r;
	color->green = rotate_g;
	color->blue = rotate_b;

	//  set image's updated dimensions
	image->height = new_height;
	image->width = new_width;

	//  set image's updated selection
	set_selection(image->select, 0, 0, new_width, new_height);
}

//  rotates an entire given image by the given parameter
void rotate_entire_image(my_image *image, char sign, int angle)
{
	//  rotate color image
	if (image->img_type == COLOR) {
		rotate_entire_color_image(image, sign, angle);
		return;
	}

	//  rotate basic image
	rotate_entire_basic_image(image, sign, angle);
}

//  crops a basic image
void crop_basic_image(my_image *image)
{
	int x1, x2, y1, y2;
	double **crop;

	//  get selection
	x1 = image->select->x1;
	y1 = image->select->y1;
	x2 = image->select->x2;
	y2 = image->select->y2;

	//  get basic image
	basic_img *basic = (basic_img *)image->img;

	//  compute cropped image
	crop = crop_matrix(basic->pixels, x1, y1, x2, y2);

	//  free previous basic image pixels
	free_matrix(basic->pixels, image->height);

	//  store cropped image
	basic->pixels = crop;
}

//  crops a color image
void crop_color_image(my_image *image)
{
	int x1, x2, y1, y2;
	double **crop_r;
	double **crop_g;
	double **crop_b;

	//  get selection
	x1 = image->select->x1;
	y1 = image->select->y1;
	x2 = image->select->x2;
	y2 = image->select->y2;

	//  get color image
	color_img *color = (color_img *)image->img;

	//  compute cropped image for every color channel
	crop_r = crop_matrix(color->red, x1, y1, x2, y2);

	crop_g = crop_matrix(color->green, x1, y1, x2, y2);

	crop_b = crop_matrix(color->blue, x1, y1, x2, y2);

	//  free previous color channels
	free_matrix(color->red, image->height);
	free_matrix(color->green, image->height);
	free_matrix(color->blue, image->height);

	//  store cropped image's channels
	color->red = crop_r;
	color->green = crop_g;
	color->blue = crop_b;
}

//  crops the loaded image
void crop_image(my_image *image)
{
	if (image->img_type == COLOR) {
		//  crop color image
		crop_color_image(image);
	} else {
		//  crop basic image
		crop_basic_image(image);
	}

	//  update cropped image's dimensions
	image->height = image->select->y2 - image->select->y1;
	image->width = image->select->x2 - image->select->x1;

	//  update cropped image's selection
	set_selection(image->select, 0, 0, image->width, image->height);
}

//  handles values outside the desired [min, max] interval
double clamp(double x, double min, double max)
{
	if (x < min)
		return min;

	if (x > max)
		return max;

	return x;
}

//  computes filtered pixel
double get_pixel(double **p, int i, int j, double kernel[3][3])
{
	double s = 0;

	s += ((double)p[i - 1][j - 1] * (kernel[0][0]));
	s += ((double)p[i - 1][j] * (kernel[0][1]));
	s += ((double)p[i - 1][j + 1] * (kernel[0][2]));

	s += ((double)p[i][j - 1] * (kernel[1][0]));
	s += ((double)p[i][j] * (kernel[1][1]));
	s += ((double)p[i][j + 1] * (kernel[1][2]));

	s += ((double)p[i + 1][j - 1] * (kernel[2][0]));
	s += ((double)p[i + 1][j] * (kernel[2][1]));
	s += ((double)p[i + 1][j + 1] * (kernel[2][2]));

	return clamp(s, 0, 255);
}

//  applies a certain filter on a color image using a given kernel matrix
void apply_filter(my_image *image, double kernel[3][3])
{
	//  get color channels
	double **red = ((color_img *)image->img)->red;
	double **green = ((color_img *)image->img)->green;
	double **blue = ((color_img *)image->img)->blue;

	//  alloc memory for the copies of the color channels
	double **copy_red = alloc_matrix(image->height, image->width);
	double **copy_green = alloc_matrix(image->height, image->width);
	double **copy_blue = alloc_matrix(image->height, image->width);

	//  copy the image's color channels
	for (int i = 0; i < image->height; ++i) {
		for (int j = 0; j < image->width; j++) {
			copy_red[i][j] = red[i][j];
			copy_green[i][j] = green[i][j];
			copy_blue[i][j] = blue[i][j];
		}
	}

	//  handle pixels in the selection that don't have neighbours
	//  ignore the pixels on the edge of the image

	//  get selection coordinates
	int start_i = image->select->y1;
	int start_j = image->select->x1;
	int end_i = image->select->y2;
	int end_j = image->select->x2;

	//  ignore the first line of the image
	if (start_i == 0)
		start_i++;

	//  ignore the first column of the image
	if (start_j == 0)
		start_j++;

	//  ignore the last line of the image
	if (end_i == image->height)
		end_i--;

	//  ignore the last column of the image
	if (end_j == image->width)
		end_j--;

	//  iterate trough all selected pixels
	for (int i = start_i; i < end_i; ++i) {
		for (int j = start_j; j < end_j; ++j) {
			//  compute & store filtered pixel for each color channel
			red[i][j] = get_pixel(copy_red, i, j, kernel);
			green[i][j] = get_pixel(copy_green, i, j, kernel);
			blue[i][j] = get_pixel(copy_blue, i, j, kernel);
		}
	}

	//  free copied color channels
	free_matrix(copy_red, image->height);
	free_matrix(copy_green, image->height);
	free_matrix(copy_blue, image->height);
}

//  creates kernel matrices with double values
void create_kernel(double kernel[3][3], double div)
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			kernel[i][j] = (double)(kernel[i][j] / div);
}

//  filters the loaded image
void apply(my_image *image, char *param)
{
	//  apply edge filter
	if (!strncmp(param, "EDGE", sizeof("EDGE") - 1)) {
		double kernel[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
		apply_filter(image, kernel);
		return;
	}

	//  apply sharpen filter
	if (!strncmp(param, "SHARPEN", sizeof("SHARPEN") - 1)) {
		double kernel[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
		apply_filter(image, kernel);
		return;
	}

	//  apply blur filter
	if (!strncmp(param, "BLUR", sizeof("BLUR") - 1)) {
		double kernel[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
		create_kernel(kernel, 9);
		apply_filter(image, kernel);
		return;
	}

	//  apply gaussian blur filter
	if (!strncmp(param, "GAUSSIAN_BLUR", sizeof("GAUSSIAN_BLUR") - 1)) {
		double kernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
		create_kernel(kernel, 16);
		apply_filter(image, kernel);
		return;
	}
}

//  saves loaded image to a text file
void save_image_text(FILE *file, my_image *image)
{
	int height, width;

	char *p = malloc(3);
	DIE(!p, "malloc p");

	//  get image's magic word

	if (image->img_type == COLOR)
		memcpy(p, "P3", strlen("P3") + 1);

	if (image->img_type == GRAYSCALE)
		memcpy(p, "P2", strlen("P2") + 1);

	if (image->img_type == BLACK_WHITE)
		memcpy(p, "P1", strlen("P1") + 1);

	//  print image's magic word to file
	fprintf(file, "%s\n", p);

	//  print image's dimensions to file
	fprintf(file, "%d %d\n", image->width, image->height);

	//  print image's pixel's max value
	if (image->img_type != BLACK_WHITE)
		fprintf(file, "%d\n", image->pixel_value);

	//  get image's dimensions
	height = image->height;
	width = image->width;

	if (image->img_type == COLOR) {
		//  get color image
		color_img *color = (color_img *)image->img;

		//  print color channels to file
		t_3_print(file, color->red, color->green, color->blue, height, width);
	} else {
		//  get basic image
		basic_img *basic = (basic_img *)image->img;

		//  print pixel matrix to file
		t_print(file, basic->pixels, height, width);
	}

	free(p);
}

//  saves loaded image to a binary file
void save_image_binary(FILE *file, my_image *image)
{
	int height, width;

	char *p = malloc(3);
	DIE(!p, "malloc p");

	//  get image's magic word

	if (image->img_type == BLACK_WHITE)
		memcpy(p, "P4", strlen("P4") + 1);

	if (image->img_type == GRAYSCALE)
		memcpy(p, "P5", strlen("P5") + 1);

	if (image->img_type == COLOR)
		memcpy(p, "P6", strlen("P6") + 1);

	//  print image's magic word to file
	fprintf(file, "%s\n", p);

	//  print image's dimensions to file
	fprintf(file, "%d %d\n", image->width, image->height);

	//  print image's pixel's max value
	if (image->img_type != BLACK_WHITE)
		fprintf(file, "%d\n", image->pixel_value);

	//  get image's dimensions
	height = image->height;
	width = image->width;

	if (image->img_type == COLOR) {
		//  get color image
		color_img *color = (color_img *)image->img;

		//  print color channels to file
		b_3_print(file, color->red, color->green, color->blue, height, width);
	} else {
		//  get basic image
		basic_img *basic = (basic_img *)image->img;

		//  print pixel matrix to file
		b_print(file, basic->pixels, height, width);
	}

	free(p);
}
