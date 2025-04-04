#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../src/stb_image.h"
#include "../src/stb_image_write.h"
#include "../src/image_processing.h"

#define TEST_WORKING_DIR "./tests/"
#define TEST_OUTPUT_FILE "test.bmp"

/* Helper function to get image dimensions */
void get_image_dimensions(const char *file_name, int *width, int *height, int *channels) {
    unsigned char *image = stbi_load(file_name, width, height, channels, 0);
    if (image == NULL) {
        fprintf(stderr, "Error: Could not load image %s.\n", file_name);
        exit(1);
    }
    stbi_image_free(image);
}

/* Test cases */
static void test_set_dir_and_output() {
    // Set the working directory
    int result = system("./build/ggpicture --set_dir tests");
    assert(result == 0);

    // Check that config.txt is in the build directory
    FILE *config = fopen("config.txt", "r");
    assert(config != NULL);
    fclose(config);

    // Set the output file
    result = system("./build/ggpicture --set_output test.bmp");
    assert(result == 0);

    printf("Test set_dir and set_output passed!\n");
}


static void test_rotate_lr() {
    int width_before, height_before, channels;
    get_image_dimensions(TEST_WORKING_DIR "input.bmp", &width_before, &height_before, &channels);

    // Rotate left
    int result = system("./build/ggpicture --rotate -l input.bmp");
    assert(result == 0);

    int width_after, height_after;
    get_image_dimensions(TEST_WORKING_DIR TEST_OUTPUT_FILE, &width_after, &height_after, &channels);
    assert(width_after == height_before);
    assert(height_after == width_before);

    printf("Test rotate left passed!\n");

    // Rotate right
    result = system("./build/ggpicture --rotate -r input.bmp");
    assert(result == 0);

    get_image_dimensions(TEST_WORKING_DIR TEST_OUTPUT_FILE, &width_after, &height_after, &channels);
    assert(width_after == height_before);
    assert(height_after == width_before);

    printf("Test rotate right passed!\n");
}

static void test_dimension_preservation_commands() {
    int width_before, height_before, channels;
    get_image_dimensions(TEST_WORKING_DIR "input.bmp", &width_before, &height_before, &channels);

    // Test commands
    const char *commands[] = {
        "--rotate -f", "--makebw", "--makevintage",
        "--setbright 10", "--setcontr 10", "--setsatur 10"
    };

    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        char command[256];
        snprintf(command, sizeof(command), "./build/ggpicture %s input.bmp", commands[i]);
        int result = system(command);
        assert(result == 0);

        int width_after, height_after;
        get_image_dimensions(TEST_WORKING_DIR TEST_OUTPUT_FILE, &width_after, &height_after, &channels);
        assert(width_after == width_before);
        assert(height_after == height_before);

        printf("Test %s passed!\n", commands[i]);
    }
}

static void test_makepixel() {
    int width_before, height_before, channels;
    get_image_dimensions(TEST_WORKING_DIR "input.bmp", &width_before, &height_before, &channels);

    // Pixelation with pixel size = 10
    int pixel_size = 10;
    char command[256];
    snprintf(command, sizeof(command), "./build/ggpicture --makepixel %d input.bmp", pixel_size);
    int result = system(command);
    assert(result == 0);

    int width_after, height_after;
    get_image_dimensions(TEST_WORKING_DIR TEST_OUTPUT_FILE, &width_after, &height_after, &channels);
    assert(width_after % pixel_size == 0);
    assert(height_after % pixel_size == 0);

    printf("Test makepixel passed!\n");
}


static int compare_images(const char *file1, const char *file2) {
    int width1, height1, channels1;
    unsigned char *image1 = stbi_load(file1, &width1, &height1, &channels1, 0);
    if (image1 == NULL) {
        fprintf(stderr, "Error: Could not load image %s.\n", file1);
        return 0;
    }

    int width2, height2, channels2;
    unsigned char *image2 = stbi_load(file2, &width2, &height2, &channels2, 0);
    if (image2 == NULL) {
        fprintf(stderr, "Error: Could not load image %s.\n", file2);
        stbi_image_free(image1);
        return 0;
    }

    if (width1 != width2 || height1 != height2 || channels1 != channels2) {
        fprintf(stderr, "Error: Image dimensions or channels do not match: %s and %s.\n", file1, file2);
        stbi_image_free(image1);
        stbi_image_free(image2);
        return 0;
    }

    size_t image_size = width1 * height1 * channels1;
    int delta = 3; // Allowable difference per pixel
    for (size_t i = 0; i < image_size; i++) {
        if (abs(image1[i] - image2[i]) > delta) {
            fprintf(stderr, "Error: Pixel mismatch at index %zu. Difference exceeds delta %d.\n", i, delta);
            stbi_image_free(image1);
            stbi_image_free(image2);
            return 0;
        }
    }

    stbi_image_free(image1);
    stbi_image_free(image2);

    return 1; // Images are identical within the allowable delta
}


static void test_rotate_full_cycle() {
    // Temporary file paths
    const char *rotated_once = TEST_WORKING_DIR "rotated_once.bmp";
    const char *rotated_twice = TEST_WORKING_DIR "rotated_twice.bmp";
    const char *rotated_thrice = TEST_WORKING_DIR "rotated_thrice.bmp";
    const char *rotated_four_times = TEST_WORKING_DIR "rotated_four_times.bmp";

    // Rotate 4 times to the right
    int result;
    result = system("./build/ggpicture --rotate -r input.bmp");
    assert(result == 0);
    remove(rotated_once);
    rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, rotated_once);
    assert(compare_images(rotated_once, "./tests/r1.bmp"));

    result = system("./build/ggpicture --rotate -r rotated_once.bmp");
    assert(result == 0);
    remove(rotated_twice);
    rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, rotated_twice);
    assert(compare_images(rotated_twice, "./tests/r2.bmp"));

    result = system("./build/ggpicture --rotate -r rotated_twice.bmp");
    assert(result == 0);
    remove(rotated_thrice);
    rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, rotated_thrice);
    assert(compare_images(rotated_thrice, "./tests/r3.bmp"));

    result = system("./build/ggpicture --rotate -r rotated_thrice.bmp");
    assert(result == 0);
    remove(rotated_four_times);
    rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, rotated_four_times);
    assert(compare_images(rotated_four_times, "./tests/r4.bmp"));

    // Compare the fourth rotation back to the original
    assert(compare_images(TEST_WORKING_DIR "input.bmp", rotated_four_times));
    printf("Test rotate full cycle (4 right rotations) passed!\n");

    // Test flip rotation
    result = system("./build/ggpicture --rotate -f input.bmp");
    assert(result == 0);
    remove(rotated_once);
    rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, rotated_once);
    assert(compare_images(rotated_once, "./tests/f1.bmp"));

    result = system("./build/ggpicture --rotate -f rotated_once.bmp");
    assert(result == 0);
    remove(rotated_twice);
    rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, rotated_twice);
    assert(compare_images(rotated_twice, "./tests/f2.bmp"));

    assert(compare_images(TEST_WORKING_DIR "input.bmp", rotated_twice));
    printf("Test rotate flip cycle (2 flips) passed!\n");

    remove(rotated_once);
    remove(rotated_twice);
    remove(rotated_thrice);
    remove(rotated_four_times);
}

static void test_repeat_operations() {
    const char *commands[] = {"--makebw", "--makepixel 10"};
    const char *external[] = {"tests/bw_img.bmp", "tests/pix_img.bmp"};
    const char *intermediate_file = TEST_WORKING_DIR "intermediate.bmp";
    const char *final_file = TEST_WORKING_DIR "final.bmp";

    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        // Apply the command the first time
        char command[256];
        snprintf(command, sizeof(command), "./build/ggpicture %s input.bmp", commands[i]);
        int result = system(command);
        assert(result == 0);
        remove(intermediate_file); // Ensure no conflict with existing file
        rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, intermediate_file);
        assert(compare_images(intermediate_file, external[i]));

        // Apply the command a second time
        snprintf(command, sizeof(command), "./build/ggpicture %s intermediate.bmp", commands[i]);
        result = system(command);
        assert(result == 0);
        remove(final_file); // Ensure no conflict with existing file
        rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, final_file);

        // Compare the intermediate and final images
        assert(compare_images(intermediate_file, external[i]));

        printf("Test repeat operation %s passed!\n", commands[i]);

        remove(intermediate_file);
        remove(final_file);
    }
}

static void test_adjustment_commands() {
    const char *commands[] = {"--setbright", "--setcontr", "--setsatur"};
    const char *external[] = {"./tests/br_input.bmp", "./tests/con_input.bmp", "./tests/sat_input.bmp"};
    const int initial_adjustment = -20;
    const int reverse_adjustment = +25;
    const char *intermediate_file = TEST_WORKING_DIR "intermediate.bmp";
    const char *final_file = TEST_WORKING_DIR "final.bmp";

    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        // Apply the initial adjustment
        char command[256];
        snprintf(command, sizeof(command), "./build/ggpicture %s %d input.bmp", commands[i], initial_adjustment);
        int result = system(command);
        assert(result == 0);
        remove(intermediate_file);
        rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, intermediate_file);
        assert(compare_images(intermediate_file, external[i]));

        // Apply the reverse adjustment
        snprintf(command, sizeof(command), "./build//ggpicture %s %d intermediate.bmp", commands[i], reverse_adjustment);
        result = system(command);
        assert(result == 0);
        remove(final_file);
        rename(TEST_WORKING_DIR TEST_OUTPUT_FILE, final_file);

        // Compare the intermediate and final images
        assert(compare_images(TEST_WORKING_DIR "input.bmp", final_file));

        printf("Test adjustment commands %s passed!\n", commands[i]);

        remove(intermediate_file);
        remove(final_file);
    }
}




/* Test runner */
int main(void) {
    printf("Running tests...\n");

    test_set_dir_and_output();
    test_rotate_lr();
    test_dimension_preservation_commands();
    test_makepixel();

    printf("=================================================\n");
    printf("Dimension tests passed successfully!\n");
	printf("=================================================\n");

    test_rotate_full_cycle();
    test_repeat_operations();
    test_adjustment_commands();

    printf("=================================================\n");
    printf("All tests passed successfully!\n");
    printf("=================================================\n");

    return EXIT_SUCCESS;
}
