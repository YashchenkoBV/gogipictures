#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "image_processing.h"

#define MAX_PATH 1024
#define CONFIG_FILE "config.txt"

void print_help() {
    printf("Image Editor - Command Line Tool\n");
    printf("--------------------------------\n");
    printf("Usage:\n");
    printf("  ./ggpicture [command] [arguments]\n");
    printf("\nCommands:\n");
    printf("  --help                     Show this help message and exit.\n");
    printf("  --set_dir <directory>      Set the working directory for input/output files.\n");
    printf("  --set_output <filename>    Set the output file name (relative to working directory).\n");
    printf("  --rotate -r/-l/-f <file>   Rotate the image right (-r), left (-l), or flip (-f).\n");
    printf("  --makebw <file>            Convert the image to black and white.\n");
    printf("  --makevintage <file>       Apply a vintage filter to the image.\n");
    printf("  --setbright +/-<value> <file>\n");
    printf("                             Adjust image brightness by the given percentage.\n");
    printf("  --setcontr +/-<value> <file>\n");
    printf("                             Adjust image contrast by the given percentage.\n");
    printf("  --setsatur +/-<value> <file>\n");
    printf("                             Adjust image saturation by the given percentage.\n");
    printf("  --makepixel <size> <file>  Pixelate the image with the given pixel size.\n");
    printf("  --blur <radius> <file>     Apply a blur effect with the given radius.\n");
    printf("\nExamples:\n");
    printf("  ./ggpicture --set_dir tests/\n");
    printf("  ./ggpicture --set_output output.bmp\n");
    printf("  ./ggpicture --rotate -r input.bmp\n");
    printf("  ./ggpicture --makepixel 10 input.bmp\n");
    printf("  ./ggpicture --blur 5 input.bmp\n");
    printf("\n");
}


// Global variables for the working directory and output file name
char working_directory[MAX_PATH] = "";
char output_file_name[MAX_PATH] = ""; // No default; determined dynamically
void get_working_directory_and_output();

// Set and store the working directory
void set_working_directory(const char *dir) {
    struct stat dir_stat;
    if (stat(dir, &dir_stat) == 0 && S_ISDIR(dir_stat.st_mode)) {


        FILE *config = fopen(CONFIG_FILE, "w");
        if (config == NULL) {
            printf("Error: Could not write to %s.\n", CONFIG_FILE);
            exit(1);
        }
        fprintf(config, "working_directory=%s\n", dir);
        fclose(config);

        printf("Working directory set to: %s\n", dir);
    } else {
        printf("Invalid directory. Please ensure the directory exists.\n");
        exit(1);
    }
}


// Set and store the output file name in config.txt
void set_output_destination(const char *file_name) {
    // Reload configuration to ensure the working directory is available
    get_working_directory_and_output();

    if (working_directory[0] == '\0') {
        printf("Error: Working directory is not set. Use --set_dir first.\n");
        exit(1);
    }

    if (strlen(file_name) >= MAX_PATH) {
        printf("Error: File name is too long.\n");
        exit(1);
    }

    char full_output_path[MAX_PATH];
    if (snprintf(full_output_path, sizeof(full_output_path), "%s/%s", working_directory, file_name) >= (int)sizeof(full_output_path)) {
        printf("Error: Full output path is too long.\n");
        exit(1);
    }

    FILE *config = fopen(CONFIG_FILE, "a"); // Append mode
    if (config == NULL) {
        printf("Error: Could not write to %s.\n", CONFIG_FILE);
        exit(1);
    }
    fprintf(config, "output_file=%s\n", full_output_path);
    fclose(config);

    strncpy(output_file_name, full_output_path, sizeof(output_file_name) - 1);
    output_file_name[sizeof(output_file_name) - 1] = '\0'; // Ensure null termination

    printf("Output destination set to: %s\n", full_output_path);
}

// Retrieve the working directory and output file name from config.txt
void get_working_directory_and_output() {
    FILE *config = fopen(CONFIG_FILE, "r");
    if (config == NULL) {
        printf("Error: No configuration found. Use --set_dir and optionally --set_output to set them.\n");
        exit(1);
    }

    char line[MAX_PATH];
    while (fgets(line, sizeof(line), config) != NULL) {
        if (strncmp(line, "working_directory=", 18) == 0) {
            strncpy(working_directory, line + 18, MAX_PATH - 1);
            working_directory[MAX_PATH - 1] = '\0';
            size_t len = strlen(working_directory);
            if (len > 0 && working_directory[len - 1] == '\n') {
                working_directory[len - 1] = '\0'; // Remove newline
            }
        } else if (strncmp(line, "output_file=", 12) == 0) {
            strncpy(output_file_name, line + 12, MAX_PATH - 1);
            output_file_name[MAX_PATH - 1] = '\0';
            size_t len = strlen(output_file_name);
            if (len > 0 && output_file_name[len - 1] == '\n') {
                output_file_name[len - 1] = '\0'; // Remove newline
            }
        }
    }
    fclose(config);

    if (working_directory[0] == '\0') {
        printf("Error: Working directory is not set. Use --set_dir to set it.\n");
        exit(1);
    }

    if (output_file_name[0] == '\0') {
        // Set default output file path
        if (snprintf(output_file_name, MAX_PATH, "%s/gogi.bmp", working_directory) >= MAX_PATH) {
            printf("Error: Default output file path is too long. Please use a shorter working directory.\n");
            exit(1);
        }
        printf("Default output file set to: %s\n", output_file_name);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: No command provided. Use --help for usage information.\n");
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0) {
        print_help();
        return 0;
    }

    if (strcmp(argv[1], "--set_dir") == 0) {
        if (argc != 3) {
            printf("Usage: ./image_editor --set_dir <dir_name>\n");
            return 1;
        }
        set_working_directory(argv[2]);
        return 0;
    }

    if (strcmp(argv[1], "--set_output") == 0) {
        if (argc != 3) {
            printf("Usage: ./image_editor --set_output <filename>\n");
            return 1;
        }
        set_output_destination(argv[2]);
        return 0;
    }

    get_working_directory_and_output();

    if (strcmp(argv[1], "--rotate") == 0) {
        if (argc != 4) {
            printf("Usage: ./image_editor --rotate -r/-l/-f <file_name>\n");
            return 1;
        }

        char file_path[MAX_PATH];
        const char *file_name = argv[3];

        // Construct file path based on working directory if not an absolute path
        if (file_name[0] != '/') {
            if (snprintf(file_path, sizeof(file_path), "%s/%s", working_directory, file_name) >= (int)sizeof(file_path)) {
                printf("Error: File path too long.\n");
                return 1;
            }
        } else {
            strncpy(file_path, file_name, sizeof(file_path) - 1);
            file_path[sizeof(file_path) - 1] = '\0'; // Ensure null termination
        }

        int rotation_type;

        if (strcmp(argv[2], "-r") == 0) {
            rotation_type = ROTATE_RIGHT;
        } else if (strcmp(argv[2], "-l") == 0) {
            rotation_type = ROTATE_LEFT;
        } else if (strcmp(argv[2], "-f") == 0) {
            rotation_type = ROTATE_FLIP;
        } else {
            printf("Invalid rotation option. Use -r, -l, or -f.\n");
            return 1;
        }

        if (process_image(file_path, rotation_type, output_file_name) != 0) {
            printf("Failed to process the image.\n");
            return 1;
        }

        printf("Image processed successfully.\n");
        return 0;
    }

    if (strcmp(argv[1], "--setbright") == 0) {
        if (argc != 4) {
            printf("Usage: ./image_editor --setbright +/-x <file_name>\n");
            return 1;
        }

        char file_path[MAX_PATH];
        const char *file_name = argv[3];

        // Construct file path based on working directory if not an absolute path
        if (file_name[0] != '/') {
            if (snprintf(file_path, sizeof(file_path), "%s/%s", working_directory, file_name) >= (int)sizeof(file_path)) {
                printf("Error: File path too long.\n");
                return 1;
            }
        } else {
            strncpy(file_path, file_name, sizeof(file_path) - 1);
            file_path[sizeof(file_path) - 1] = '\0'; // Ensure null termination
        }

        int brightness_adjustment = strtol(argv[2], NULL, 10); // Correct parsing of +/-x

        if (adjust_brightness(file_path, brightness_adjustment, output_file_name) != 0) {
            printf("Failed to adjust the brightness.\n");
            return 1;
        }

        printf("Brightness adjusted successfully.\n");
        return 0;
    }

    if (strcmp(argv[1], "--setcontr") == 0) {
        if (argc != 4) {
            printf("Usage: ./image_editor --setcontr +/-x <file_name>\n");
            return 1;
        }

        char file_path[MAX_PATH];
        const char *file_name = argv[3];

        // Construct file path based on working directory if not an absolute path
        if (file_name[0] != '/') {
            if (snprintf(file_path, sizeof(file_path), "%s/%s", working_directory, file_name) >= (int)sizeof(file_path)) {
                printf("Error: File path too long.\n");
                return 1;
            }
        } else {
            strncpy(file_path, file_name, sizeof(file_path) - 1);
            file_path[sizeof(file_path) - 1] = '\0'; // Ensure null termination
        }

        int contrast_adjustment = strtol(argv[2], NULL, 10); // Parse +/-x

        if (adjust_contrast(file_path, contrast_adjustment, output_file_name) != 0) {
            printf("Failed to adjust the contrast.\n");
            return 1;
        }

        printf("Contrast adjusted successfully.\n");
        return 0;
    }

    if (strcmp(argv[1], "--makebw") == 0) {
        if (argc != 3) {
            printf("Usage: ./image_editor --makebw <file_name>\n");
            return 1;
        }

        char file_path[MAX_PATH];
        const char *file_name = argv[2];

        // Construct file path based on working directory if not an absolute path
        if (file_name[0] != '/') {
            if (snprintf(file_path, sizeof(file_path), "%s/%s", working_directory, file_name) >= (int)sizeof(file_path)) {
                printf("Error: File path too long.\n");
                return 1;
            }
        } else {
            strncpy(file_path, file_name, sizeof(file_path) - 1);
            file_path[sizeof(file_path) - 1] = '\0'; // Ensure null termination
        }

        if (make_black_and_white(file_path, output_file_name) != 0) {
            printf("Failed to convert the image to black and white.\n");
            return 1;
        }

        printf("Image converted to black and white successfully.\n");
        return 0;
    }

    if (strcmp(argv[1], "--makevintage") == 0) {
        if (argc != 3) {
            printf("Usage: ./image_editor --makevintage <file_name>\n");
            return 1;
        }

        char file_path[MAX_PATH];
        const char *file_name = argv[2];

        // Construct file path based on working directory if not an absolute path
        if (file_name[0] != '/') {
            if (snprintf(file_path, sizeof(file_path), "%s/%s", working_directory, file_name) >= (int)sizeof(file_path)) {
                printf("Error: File path too long.\n");
                return 1;
            }
        } else {
            strncpy(file_path, file_name, sizeof(file_path) - 1);
            file_path[sizeof(file_path) - 1] = '\0'; // Ensure null termination
        }

        if (make_vintage(file_path, output_file_name) != 0) {
            printf("Failed to apply the vintage effect.\n");
            return 1;
        }

        printf("Vintage effect applied successfully.\n");
        return 0;
    }

    if (strcmp(argv[1], "--setsatur") == 0) {
        if (argc != 4) {
            printf("Usage: ./image_editor --setsatur +/-x <file_name>\n");
            return 1;
        }

        char file_path[MAX_PATH];
        const char *file_name = argv[3];

        // Construct file path based on working directory if not an absolute path
        if (file_name[0] != '/') {
            if (snprintf(file_path, sizeof(file_path), "%s/%s", working_directory, file_name) >= (int)sizeof(file_path)) {
                printf("Error: File path too long.\n");
                return 1;
            }
        } else {
            strncpy(file_path, file_name, sizeof(file_path) - 1);
            file_path[sizeof(file_path) - 1] = '\0'; // Ensure null termination
        }

        int saturation_adjustment = strtol(argv[2], NULL, 10); // Parse +/-x

        if (adjust_saturation(file_path, saturation_adjustment, output_file_name) != 0) {
            printf("Failed to adjust the saturation.\n");
            return 1;
        }

        printf("Saturation adjusted successfully.\n");
        return 0;
    }

    if (strcmp(argv[1], "--blur") == 0) {
        if (argc != 4) {
            printf("Usage: ./image_editor --blur <radius> <file_name>\n");
            return 1;
        }

        char file_path[MAX_PATH];
        const char *file_name = argv[3];
        int radius = atoi(argv[2]);

        if (radius <= 0) {
            printf("Error: Radius must be a positive integer.\n");
            return 1;
        }

        // Construct file path based on working directory if not an absolute path
        if (file_name[0] != '/') {
            if (snprintf(file_path, sizeof(file_path), "%s/%s", working_directory, file_name) >= (int)sizeof(file_path)) {
                printf("Error: File path too long.\n");
                return 1;
            }
        } else {
            strncpy(file_path, file_name, sizeof(file_path) - 1);
            file_path[sizeof(file_path) - 1] = '\0'; // Ensure null termination
        }

        if (blur_image(file_path, radius, output_file_name) != 0) {
            printf("Failed to blur the image.\n");
            return 1;
        }

        printf("Image blurred successfully.\n");
        return 0;
    }

    if (strcmp(argv[1], "--makepixel") == 0) {
        if (argc != 4) {
            printf("Usage: ./image_editor --makepixel <pixel_size> <file_name>\n");
            return 1;
        }

        int pixel_size = atoi(argv[2]);
        if (pixel_size <= 0) {
            printf("Error: Pixel size must be a positive integer.\n");
            return 1;
        }

        char file_path[MAX_PATH];
        const char *file_name = argv[3];

        // Construct file path based on working directory if not an absolute path
        if (file_name[0] != '/') {
            if (snprintf(file_path, sizeof(file_path), "%s/%s", working_directory, file_name) >= (int)sizeof(file_path)) {
                printf("Error: File path too long.\n");
                return 1;
            }
        } else {
            strncpy(file_path, file_name, sizeof(file_path) - 1);
            file_path[sizeof(file_path) - 1] = '\0'; // Ensure null termination
        }

        if (make_pixelated(file_path, pixel_size, output_file_name) != 0) {
            printf("Failed to pixelate the image.\n");
            return 1;
        }

        printf("Image pixelated successfully.\n");
        return 0;
    }


    printf("Invalid command. Use --set_dir, --set_output, --rotate, --setbright or else.\n");
    return 1;
}
