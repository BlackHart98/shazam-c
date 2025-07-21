CC = clang
CFLAGS = -fsanitize=address -g -O0 -Iinclude


BUILD_PATH = ./build

OBJS = $(BUILD_PATH)/main.o $(BUILD_PATH)/ffmpeg.o


build: create_dir $(BUILD_PATH)/main


build-clean: clean create_dir $(BUILD_PATH)/main


$(BUILD_PATH)/main: $(OBJS)
	@echo "Compiling $(BUILD_PATH)/main ..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(BUILD_PATH)/main
	@echo "Compilation successful."


$(BUILD_PATH)/main.o: 
	@$(CC) $(CFLAGS) -c ./src/main.c -o $(BUILD_PATH)/main.o

$(BUILD_PATH)/ffmpeg.o:
	@$(CC) $(CFLAGS) -c ./src/ffmpeg.c -o $(BUILD_PATH)/ffmpeg.o

clean:
	@rm -r $(BUILD_PATH)/*

create_dir:
	@mkdir -p $(BUILD_PATH)/







