CC = clang
CFLAGS = -fsanitize=address -g -O0 -Iinclude


BUILD_PATH = ./build

OBJS = $(BUILD_PATH)/main.o $(BUILD_PATH)/ffmpeg.o


all: $(BUILD_PATH)/main


$(BUILD_PATH)/main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILD_PATH)/main


$(BUILD_PATH)/main.o: 
	$(CC) $(CFLAGS) -c ./src/main.c -o $(BUILD_PATH)/main.o

$(BUILD_PATH)/ffmpeg.o:
	$(CC) $(CFLAGS) -c ./src/ffmpeg.c -o $(BUILD_PATH)/ffmpeg.o

clean:
	rm -rf $(BUILD_PATH)/*




