#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>

#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<fcntl.h>
#include<signal.h>


#include"ffmpeg.h"
#include"utils.h"
#include<base64.h>



#define CHILD_PROCESS                    0
#define AUDIO_BUFFER_SIZE                1024
#define FILEPATH_BUFFER_SIZE             4096


void _start_recording(
    const char *media_format, 
    const char *audio_source, 
    const char *audio_file_target
){
    int ret = execlp(
        "ffmpeg", "ffmpeg", 
        "-hide_banner", 
        "-f", media_format, "-i", audio_source,
        "-ac", "1", "-f", "s16le", "-acodec", "pcm_s16le", "-ar", "44100",
        "-y", audio_file_target,
        NULL);
    if (ret < 0)
    perror("Failed to execute ffmpeg.");
    exit(1);
}


int ffmpeg_record_audio_from_source(
    const char *media_format, 
    const char *audio_source, 
    const char *audio_file_target,
    int recording_time 
){
    pid_t id = fork();
    if (id == CHILD_PROCESS){
        _start_recording(
            media_format, 
            audio_source, 
            audio_file_target);
    } else if (id == -1){
        printf("Failed to create child process.\n");
        return 1;
    } else if (id != CHILD_PROCESS){
        sleep(recording_time);
        if (kill(id, SIGTERM) == -1) {
            printf("Could not issue SIGTERM.\n");
        }

        wait(NULL);
        printf("Parent: done.\n");
        printf("hello world ffmpeg! from main process %d\n.", id);
    }
    return 0;
}


int convert_audio_to_dat(
    const char *audio_file_source, 
    const char *audio_target_dat, 
    const char *recording_time
){
    pid_t id = fork();
    if (id == CHILD_PROCESS){
        int ret = execlp(
            "ffmpeg", "ffmpeg",
            "-ss", "10", "-t", recording_time,
            "-i", audio_file_source,
            "-ac", "1", "-f", "s16le", "-acodec", "pcm_s16le", "-ar", "44100",
            "-y", audio_target_dat,
            NULL);
        if (ret == -1){
            perror("Failed to read audio file.");
            exit(1);
        }
        exit(0);
    } else if (id == -1){
        perror("Failed to create child process.");
        return 1;
    } else {
        wait(NULL);
        printf("Hello world.");
    }
    return 0;
}


char* parse_dat_file(const char* dat_file){
    FILE *fptr = fopen(dat_file, "rb"); 
    try_or_return_msg(fptr, NULL, NULL, "Could not open this file."); 

    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    rewind(fptr); 

    unsigned char *buffer = malloc(file_size);
    if (buffer == NULL) {
        perror("Out of memory.");
        fclose(fptr);
        return NULL;
    }

    printf("file size: %liKiB\n", file_size / 1024);

    size_t read = fread(buffer, 1, file_size, fptr);

    for (size_t i = 0; i < read; i++) {
        printf("%02X ", buffer[i]);  // Print in hex
        if ((i + 1) % 16 == 0) {
            printf("\n");
            break;
        }
    }
    char* result = encode64(buffer, read);
    fclose(fptr);free(buffer);
    return result;
}