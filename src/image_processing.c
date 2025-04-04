#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "image_processing.h"
#include <math.h>

extern char working_directory[];

int process_image(const char *file_name, int rotation_type, const char *output_file_name) {
    int width, height, channels;
    unsigned char *image = stbi_load(file_name, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Error: Could not load the image from %s.\n", file_name);
        return 1;
    }

    unsigned char *rotated_image = NULL;

    if (rotation_type == ROTATE_RIGHT) {
        rotated_image = rotate_right(image, width, height, channels);
    } else if (rotation_type == ROTATE_LEFT) {
        rotated_image = rotate_left(image, width, height, channels);
    } else if (rotation_type == ROTATE_FLIP) {
        rotated_image = rotate_flip(image, width, height, channels);
    }

    if (rotated_image == NULL) {
        printf("Error: Rotation failed.\n");
        stbi_image_free(image);
        return 1;
    }

    if (!stbi_write_bmp(output_file_name, rotation_type == ROTATE_FLIP ? width : height,
                        rotation_type == ROTATE_FLIP ? height : width, channels, rotated_image)) {
        printf("Error: Could not save the rotated image to %s.\n", output_file_name);
        free(rotated_image);
        stbi_image_free(image);
        return 1;
                        }

    printf("Rotated image saved to %s\n", output_file_name);

    free(rotated_image);
    stbi_image_free(image);
    return 0;
}



unsigned char *rotate_right(const unsigned char *image, int width, int height, int channels) {
    unsigned char *rotated = malloc(width * height * channels);
    if (rotated == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                rotated[(x * height + (height - y - 1)) * channels + c] =
                    image[(y * width + x) * channels + c];
            }
        }
    }

    return rotated;
}

unsigned char *rotate_left(const unsigned char *image, int width, int height, int channels) {
    unsigned char *rotated = malloc(width * height * channels);
    if (rotated == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                rotated[((width - x - 1) * height + y) * channels + c] =
                    image[(y * width + x) * channels + c];
            }
        }
    }

    return rotated;
}

unsigned char *rotate_flip(const unsigned char *image, int width, int height, int channels) {
    unsigned char *flipped = malloc(width * height * channels);
    if (flipped == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                flipped[((height - y - 1) * width + (width - x - 1)) * channels + c] =
                    image[(y * width + x) * channels + c];
            }
        }
    }

    return flipped;
}

int adjust_brightness(const char *file_name, int percentage, const char *output_file_name) {
    int width, height, channels;
    unsigned char *image = stbi_load(file_name, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Error: Could not load the image from %s.\n", file_name);
        return 1;
    }

    for (int i = 0; i < width * height * channels; i++) {
        int adjusted_value = image[i] + (image[i] * percentage / 100);
        image[i] = (unsigned char)(adjusted_value < 0 ? 0 : (adjusted_value > 255 ? 255 : adjusted_value));
    }

    if (!stbi_write_bmp(output_file_name, width, height, channels, image)) {
        printf("Error: Could not save the adjusted image to %s.\n", output_file_name);
        stbi_image_free(image);
        return 1;
    }

    printf("Brightness-adjusted image saved to %s\n", output_file_name);
    stbi_image_free(image);
    return 0;
}

int adjust_contrast(const char *file_name, int percentage, const char *output_file_name) {
    int width, height, channels;
    unsigned char *image = stbi_load(file_name, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Error: Could not load the image from %s.\n", file_name);
        return 1;
    }

    float factor = 1.0f + (percentage / 100.0f);
    int midpoint = 128;

    for (int i = 0; i < width * height * channels; i++) {
        int adjusted_value = midpoint + (image[i] - midpoint) * factor;
        image[i] = (unsigned char)(adjusted_value < 0 ? 0 :
                                   (adjusted_value > 255 ? 255 : adjusted_value));
    }

    if (!stbi_write_bmp(output_file_name, width, height, channels, image)) {
        printf("Error: Could not save the adjusted image to %s.\n", output_file_name);
        stbi_image_free(image);
        return 1;
    }

    printf("Contrast-adjusted image saved to %s\n", output_file_name);
    stbi_image_free(image);
    return 0;
}

int make_black_and_white(const char *file_name, const char *output_file_name) {
    int width, height, channels;
    unsigned char *image = stbi_load(file_name, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Error: Could not load the image from %s.\n", file_name);
        return 1;
    }

    if (channels < 3) {
        printf("Error: Image does not have enough color channels for RGB.\n");
        stbi_image_free(image);
        return 1;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            unsigned char r = image[idx];
            unsigned char g = image[idx + 1];
            unsigned char b = image[idx + 2];
            unsigned char luminance = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
            image[idx] = luminance;
            image[idx + 1] = luminance;
            image[idx + 2] = luminance;
        }
    }

    if (!stbi_write_bmp(output_file_name, width, height, channels, image)) {
        printf("Error: Could not save the black-and-white image to %s.\n", output_file_name);
        stbi_image_free(image);
        return 1;
    }

    printf("Black-and-white image saved to %s\n", output_file_name);
    stbi_image_free(image);
    return 0;
}

int make_vintage(const char *file_name, const char *output_file_name) {
    int width, height, channels;
    unsigned char *image = stbi_load(file_name, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Error: Could not load the image from %s.\n", file_name);
        return 1;
    }

    if (channels < 3) {
        printf("Error: Image does not have enough color channels for RGB.\n");
        stbi_image_free(image);
        return 1;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            unsigned char r = image[idx];
            unsigned char g = image[idx + 1];
            unsigned char b = image[idx + 2];

            int new_r = (int)(0.393 * r + 0.769 * g + 0.189 * b);
            int new_g = (int)(0.349 * r + 0.686 * g + 0.168 * b);
            int new_b = (int)(0.272 * r + 0.534 * g + 0.131 * b);

            image[idx] = (unsigned char)(new_r > 255 ? 255 : new_r);
            image[idx + 1] = (unsigned char)(new_g > 255 ? 255 : new_g);
            image[idx + 2] = (unsigned char)(new_b > 255 ? 255 : new_b);
        }
    }

    if (!stbi_write_bmp(output_file_name, width, height, channels, image)) {
        printf("Error: Could not save the vintage image to %s.\n", output_file_name);
        stbi_image_free(image);
        return 1;
    }

    printf("Vintage image saved to %s\n", output_file_name);
    stbi_image_free(image);
    return 0;
}

int adjust_saturation(const char *file_name, int percentage, const char *output_file_name) {
    int width, height, channels;
    unsigned char *image = stbi_load(file_name, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Error: Could not load the image from %s.\n", file_name);
        return 1;
    }

    if (channels < 3) {
        printf("Error: Image does not have enough color channels for RGB.\n");
        stbi_image_free(image);
        return 1;
    }

    float factor = 1.0f + (percentage / 100.0f); // Compute the saturation adjustment factor

    for (int y = 0; y < height; y++) {
        for (int xk = 0; xk < width; xk++) {
            int idx = (y * width + xk) * channels;

            float r = image[idx] / 255.0f;
            float g = image[idx + 1] / 255.0f;
            float b = image[idx + 2] / 255.0f;

            // Convert RGB to HSL
            float max = fmaxf(r, fmaxf(g, b));
            float min = fminf(r, fminf(g, b));
            float delta = max - min;

            float h = 0.0f, s = 0.0f, l = (max + min) / 2.0f;

            if (delta != 0.0f) {
                s = l < 0.5f ? (delta / (max + min)) : (delta / (2.0f - max - min));

                if (max == r) {
                    h = (g - b) / delta + (g < b ? 6.0f : 0.0f);
                } else if (max == g) {
                    h = (b - r) / delta + 2.0f;
                } else {
                    h = (r - g) / delta + 4.0f;
                }

                h /= 6.0f;
            }

            // Adjust saturation
            s *= factor;
            if (s > 1.0f) s = 1.0f;
            if (s < 0.0f) s = 0.0f;

            // Convert HSL back to RGB
            float c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
            float x = c * (1.0f - fabsf(fmodf(h * 6.0f, 2.0f) - 1.0f));
            float m = l - c / 2.0f;

            float r_prime, g_prime, b_prime;

            if (h < 1.0f / 6.0f) {
                r_prime = c; g_prime = x; b_prime = 0.0f;
            } else if (h < 2.0f / 6.0f) {
                r_prime = x; g_prime = c; b_prime = 0.0f;
            } else if (h < 3.0f / 6.0f) {
                r_prime = 0.0f; g_prime = c; b_prime = x;
            } else if (h < 4.0f / 6.0f) {
                r_prime = 0.0f; g_prime = x; b_prime = c;
            } else if (h < 5.0f / 6.0f) {
                r_prime = x; g_prime = 0.0f; b_prime = c;
            } else {
                r_prime = c; g_prime = 0.0f; b_prime = x;
            }

            image[idx] = (unsigned char)((r_prime + m) * 255.0f);
            image[idx + 1] = (unsigned char)((g_prime + m) * 255.0f);
            image[idx + 2] = (unsigned char)((b_prime + m) * 255.0f);
        }
    }

    if (!stbi_write_bmp(output_file_name, width, height, channels, image)) {
        printf("Error: Could not save the saturation-adjusted image to %s.\n", output_file_name);
        stbi_image_free(image);
        return 1;
    }

    printf("Saturation-adjusted image saved to %s\n", output_file_name);
    stbi_image_free(image);
    return 0;
}

int blur_image(const char *file_name, int radius, const char *output_file_name) {
    int width, height, channels;
    unsigned char *image = stbi_load(file_name, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Error: Could not load the image from %s.\n", file_name);
        return 1;
    }

    if (channels < 3) {
        printf("Error: Image does not have enough color channels for RGB.\n");
        stbi_image_free(image);
        return 1;
    }

    int kernel_size = 2 * radius + 1;
    float *kernel = malloc(kernel_size * sizeof(float));
    if (kernel == NULL) {
        printf("Error: Could not allocate memory for the kernel.\n");
        stbi_image_free(image);
        return 1;
    }

    float sigma = radius / 2.0f;
    float sum = 0.0f;

    for (int i = 0; i < kernel_size; i++) {
        float x = i - radius;
        kernel[i] = expf(-x * x / (2.0f * sigma * sigma));
        sum += kernel[i];
    }

    for (int i = 0; i < kernel_size; i++) {
        kernel[i] /= sum;
    }

    unsigned char *temp_image = malloc(width * height * channels);
    if (temp_image == NULL) {
        printf("Error: Could not allocate memory for temporary image.\n");
        free(kernel);
        stbi_image_free(image);
        return 1;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                float value = 0.0f;
                for (int k = -radius; k <= radius; k++) {
                    int xk = x + k;
                    if (xk >= 0 && xk < width) {
                        value += image[(y * width + xk) * channels + c] * kernel[k + radius];
                    }
                }
                temp_image[(y * width + x) * channels + c] = (unsigned char)(value);
            }
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                float value = 0.0f;
                for (int k = -radius; k <= radius; k++) {
                    int yk = y + k;
                    if (yk >= 0 && yk < height) {
                        value += temp_image[(yk * width + x) * channels + c] * kernel[k + radius];
                    }
                }
                image[(y * width + x) * channels + c] = (unsigned char)(value);
            }
        }
    }

    free(kernel);
    free(temp_image);

    if (!stbi_write_bmp(output_file_name, width, height, channels, image)) {
        printf("Error: Could not save the blurred image to %s.\n", output_file_name);
        stbi_image_free(image);
        return 1;
    }

    printf("Blurred image saved to %s\n", output_file_name);
    stbi_image_free(image);
    return 0;
}


int make_pixelated(const char *file_name, int pixel_size, const char *output_file_name) {
    int width, height, channels;

    // Load the image
    unsigned char *image = stbi_load(file_name, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Error: Could not load the image from %s.\n", file_name);
        return 1;
    }

    // Calculate the effective width and height to ensure divisibility by pixel_size
    int effective_width = (width / pixel_size) * pixel_size;
    int effective_height = (height / pixel_size) * pixel_size;

    // Allocate memory for the pixelated image
    unsigned char *pixelated_image = (unsigned char *)malloc(effective_width * effective_height * channels);
    if (pixelated_image == NULL) {
        printf("Error: Could not allocate memory for the pixelated image.\n");
        stbi_image_free(image);
        return 1;
    }

    // Pixelation logic
    for (int y = 0; y < effective_height; y += pixel_size) {
        for (int x = 0; x < effective_width; x += pixel_size) {
            // Calculate the average color of the current block
            int r = 0, g = 0, b = 0, a = 0, count = 0;

            for (int dy = 0; dy < pixel_size; dy++) {
                for (int dx = 0; dx < pixel_size; dx++) {
                    int px = x + dx;
                    int py = y + dy;

                    if (px < width && py < height) {
                        int idx = (py * width + px) * channels;
                        r += image[idx];
                        g += image[idx + 1];
                        b += image[idx + 2];
                        if (channels == 4) {
                            a += image[idx + 3];
                        }
                        count++;
                    }
                }
            }

            // Average the colors
            r /= count;
            g /= count;
            b /= count;
            if (channels == 4) {
                a /= count;
            }

            // Assign the average color to the entire block
            for (int dy = 0; dy < pixel_size; dy++) {
                for (int dx = 0; dx < pixel_size; dx++) {
                    int px = x + dx;
                    int py = y + dy;

                    if (px < effective_width && py < effective_height) {
                        int idx = (py * effective_width + px) * channels;
                        pixelated_image[idx] = r;
                        pixelated_image[idx + 1] = g;
                        pixelated_image[idx + 2] = b;
                        if (channels == 4) {
                            pixelated_image[idx + 3] = a;
                        }
                    }
                }
            }
        }
    }

    // Save the pixelated image
    if (!stbi_write_bmp(output_file_name, effective_width, effective_height, channels, pixelated_image)) {
        printf("Error: Could not save the pixelated image to %s.\n", output_file_name);
        free(pixelated_image);
        stbi_image_free(image);
        return 1;
    }

    printf("Pixelated image saved to %s\n", output_file_name);

    // Free allocated memory
    free(pixelated_image);
    stbi_image_free(image);
    return 0;
}

