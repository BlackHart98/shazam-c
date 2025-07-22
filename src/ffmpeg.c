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



#define CHILD_PROCESS                 0
#define AUDIO_BUFFER_SIZE             1024



void _start_recording(
    const char *media_format, 
    const char *audio_source, 
    const char *audio_file_target
){
    int ret = execlp(
        "ffmpeg", 
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
    int id = fork();
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
            printf("Could not issue SIGTERM");
        }

        // Wait for child to exit
        wait(NULL);
        printf("Parent: done.\n");
        printf("hello world ffmpeg! from main process %d\n.", id);
    }
    return 0;
}


int convert_audio_to_dat(){}


int recording_ffmpeg(const char *audio_source){
    char buffer[AUDIO_BUFFER_SIZE];
    return 1;
}



