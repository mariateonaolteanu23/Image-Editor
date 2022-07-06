#ifndef IMAGE_UTTILS_
#define IMAGE_UTTILS_

enum file {TEXT = 0, BINARY = 1};
enum image_type {BLACK_WHITE = 4, GRAYSCALE = 5, COLOR = 6};

#define MAX_LINE_SIZE 255

//  stores image's selection
typedef struct {
	int x1;
	int x2;
	int y1;
	int y2;
} my_select;

//  stores image's data
typedef struct{
	//  image's input file_type
	enum file file_type;
	//  image's type
	enum image_type img_type;
	//  image's number of columns
	int width;
	//  image's number of rows
	int height;
	//  image's max pixel value
	int pixel_value;
	//  image's pixels (the actual image)
	void *img;
	//  image's current selection
	my_select *select;
} my_image;

//  color image's 3 color channels
typedef struct{
	double **red;
	double **green;
	double **blue;
} color_img;

//  basic image's pixels (matrix of pixels)
typedef struct{
	double **pixels;
} basic_img;

bool is_empty(my_image *image);

void init_image_data(my_image *image);

void load_image(FILE *file, my_image *image);

void free_image_data(my_image *image);

void set_selection(my_select *select, int x1, int y1, int x2, int y2);

void rotate_image_selection(my_image *image, char sign, int angle);

void rotate_entire_image(my_image *image, char sign, int angle);

void crop_image(my_image *image);

void apply(my_image *image, char *args);

void save_image_text(FILE *file, my_image *image);

void save_image_binary(FILE *file, my_image *image);

#endif /* IMAGE_UTTILS_ */
