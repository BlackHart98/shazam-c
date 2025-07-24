# shazam-c
Shazam music from the CLI


This is largely inspired by this project https://github.com/loiccoyle/shazam-cli


## Prerequisites
1. Ensure to have ffmpeg and curl installed
2. Get API key from https://rapidapi.com/apidojo/api/shazam


## Build binary
1. Run this
```sh
make release
```

## How to run
1. Shazam from local file
```sh
./build/shazam-clone <file_path>
```
2. Shazam from audio source
```sh
./build/shazam-clone
```
3. For help 
```sh
./build/shazam-clone -h
```



> [!Note]
> This has only been tested to work for macOs m1 
> I will remove dependencies on relative paths