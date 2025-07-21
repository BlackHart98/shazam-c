# Enable all sanitizers
clang -fsanitize=address,undefined -g -O0 main.c

# Just memory errors
clang -fsanitize=address -fno-omit-frame-pointer -g main.c

export MallocStackLogging=1
export MallocScribble=1
export MallocPreScribble=1
export MallocGuardEdges=1
export MallocCheckHeapStart=1

./bin/main