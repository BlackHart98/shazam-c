CC = clang
CFLAGS = -fsanitize=address -g -O0 -Iinclude -Wall -Werror


BUILD_PATH = ./build



OBJS = $(BUILD_PATH)/main.o $(BUILD_PATH)/ffmpeg.o $(BUILD_PATH)/utils.o $(BUILD_PATH)/base64.o

# TESTOBJS = $(BUILD_PATH)/ffmpeg.o $(BUILD_PATH)/utils.o

build: create_dir $(BUILD_PATH)/main

tests: clean create_dir $(TESTOBJS)

build-clean: clean create_dir $(BUILD_PATH)/main


$(BUILD_PATH)/main: $(OBJS)
	@echo "Compiling $(BUILD_PATH)/main ..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(BUILD_PATH)/main
	@echo "Compilation successful."


$(BUILD_PATH)/main.o: 
	@$(CC) $(CFLAGS) -c ./src/main.c -o $(BUILD_PATH)/main.o

$(BUILD_PATH)/ffmpeg.o:
	@$(CC) $(CFLAGS) -c ./src/ffmpeg.c -o $(BUILD_PATH)/ffmpeg.o

$(BUILD_PATH)/utils.o:
	@$(CC) $(CFLAGS) -c ./src/utils.c -o $(BUILD_PATH)/utils.o

$(BUILD_PATH)/base64.o:
	@$(CC) $(CFLAGS) -c ./src/base64.c -o $(BUILD_PATH)/base64.o

clean:
	@rm -rf $(BUILD_PATH)/

create_dir:
	@mkdir -p $(BUILD_PATH)/







