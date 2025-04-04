all: build/ggpicture

build/ggpicture: build/src/main.o build/src/image_processing.o
	gcc build/src/main.o build/src/image_processing.o -o build/ggpicture -lm

build/src/main.o: src/main.c
	mkdir -p build/src
	gcc -Wall -Wextra -c src/main.c -o build/src/main.o

build/src/image_processing.o: src/image_processing.c src/image_processing.h
	mkdir -p build/src
	gcc -Wall -Wextra -c src/image_processing.c -o build/src/image_processing.o

test: build/run_tests
	./build/run_tests
	rm -f config.txt

build/run_tests: build/tests/test_main.o build/src/image_processing.o
	gcc build/tests/test_main.o build/src/image_processing.o -o build/run_tests -lm

build/tests/test_main.o: tests/test_main.c src/image_processing.h src/stb_image.h src/stb_image_write.h
	mkdir -p build/tests
	gcc -Wall -Wextra -I./src -c tests/test_main.c -o build/tests/test_main.o

clean:
	rm -rf build
