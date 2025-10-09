# shazam-c
Shazam music from the CLI


This is largely inspired by this project https://github.com/loiccoyle/shazam-cli


## Prerequisites
1. Ensure to have ffmpeg and curl installed
2. Get API key from https://rapidapi.com/apidojo/api/shazam
3. Run the command below to build recipe executable
On Unix-like system
```sh
cc nob.c -o nob
```


## Build binary
1. Run this 
```sh
./nob
```

## How to run
1. Shazam from local file
```sh
sh run_test.sh
```



> [!Note]
> This has only been tested to work for macOs m1 
> I will remove dependencies on relative paths