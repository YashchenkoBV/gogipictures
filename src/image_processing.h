#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <limits.h>

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

#define ROTATE_RIGHT 1
#define ROTATE_LEFT 2
#define ROTATE_FLIP 3

// Extern declaration for the global working directory
extern char working_directory[MAX_PATH];

// Main processing functions
int process_image(const char *file_name, int rotation_type, const char *output_file_name);
int adjust_brightness(const char *file_name, int percentage, const char *output_file_name);
int adjust_contrast(const char *file_name, int percentage, const char *output_file_name);
int make_black_and_white(const char *file_name, const char *output_file_name);
int make_vintage(const char *file_name, const char *output_file_name);
int adjust_saturation(const char *file_name, int percentage, const char *output_file_name);
int blur_image(const char *file_name, int radius, const char *output_file_name);
int make_pixelated(const char *file_name, int pixel_size, const char *output_file_name);


// Rotation functions
unsigned char *rotate_right(const unsigned char *image, int width, int height, int channels);
unsigned char *rotate_left(const unsigned char *image, int width, int height, int channels);
unsigned char *rotate_flip(const unsigned char *image, int width, int height, int channels);

#endif
