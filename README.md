[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/q4OE-XW1)
# GoGiPictures

A command-line image editor. Provides opportunity to apply different filters to .bmp images and change their visual parameters. Video link: https://youtu.be/grqgTqurPCI

## Features

1. Rotation
   - Rotating the image by 90 degrees to either side or by 180 degrees
   - Image is successfully rotated, the dimensions and quality being preserved

2. Adjucting visual parameters
   - Changing image's saturation/contrast/brightness by any percentage
   - The needed parameter (saturation/contrast/brightness) is successully adjucted

3. Applying filters
   - Making image black and white or adding vintage effect (sepia) to the image
   - The aforementioned filters are successully applied

4. Pixelation
   - Creating pixel-art of the image, size of pixels being chosen by user
   - Image becomes pixelated with the needed size of pixel-squares

5. Blurring
   - Applying a Gaussian blur algorithm, the radius is given by user
   - Image is blurred with the needed radius
  
6. Choosing working directory and output destination
   - User sets working directory and the file, he wants to save output to
   - The output is saved to the given destination

## Dependencies

- GCC
- Make
- stb_image.h, stb_image_write.h

## Build Instructions

1. Clone the repository:
```bash
git clone 
cd project-2024c-YashchenkoBV
```

2. Build the project:
```bash
make
```

3. Run tests:
```bash
make test
```

4. Go into /build directory to use program:
```bash
cd build
```

## Usage Examples

1. Set working directory
```bash
./ggpicture --set_dir ../workdir
```

2. Set output file (optional, otherwise saved to gogi.bmp in working directory):
```bash
./ggpicture --set_output output.bmp
```

3. Rotate to the right:
```bash
./ggpicture --rotate -r input.bmp
```

4. Change saturation:
```bash
./ggpicture --setsatur +50 input.bmp
```

5. Apply vintage filter:
```bash
./ggpicture --makevintage input.bmp
```

6. Blur:
```bash
./ggpicture --blur 5 input.bmp
```

7. See more:
```bash
./ggpicture --help
```
