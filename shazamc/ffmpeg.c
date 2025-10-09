#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "shazamc.h"

#if defined(__APPLE__) || defined(__linux__) || defined(FreeBSD)
    #define OS_UNIX_LIKE
    #include <fcntl.h>
    #include <signal.h>
    #include <sys/wait.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#else
    #error Unsupported platform!
#endif


#define CHILD_PROCESS                       0
#define AUDIO_BUFFER_SIZE                   1024
#define FILEPATH_BUFFER_SIZE                4096
#define PROCESS_INIT_FAILED                 -1
#define SUBPROCESS_FAILED                   -1
#define FAILED_TO_SIGTERM                   -1



#if defined(__APPLE__)
    #define MEDIA_FORMAT                "avfoundation"
    #define AUDIO_SOURCE                ":1"
#elif defined(__linux__)
    #define MEDIA_FORMAT                "pulse"
    #define AUDIO_SOURCE                "default"
#endif

#define SAMPLE_RATE                     44100
#define TARGET_SAMPLES                  44100 * 2 * 10
#define AUDIO_INBUF_SIZE                20480

void shazamc_ffmpeg_start_recording(const char *audio_file_target){
    int ret = execlp(
        "ffmpeg", "ffmpeg", 
        "-hide_banner", 
        "-loglevel", "quiet",
        "-f", MEDIA_FORMAT, "-i", AUDIO_SOURCE,
        "-ac", "1", "-f", "s16le", "-acodec", "pcm_s16le", "-ar", "44100",
        "-y", audio_file_target,
        NULL);
    if (0 > ret) 
        perror("Failed to execute ffmpeg.");
    exit(1);
}



int shazamc_ffmpeg_record_audio_from_source(const char *audio_file_target, int recording_time){
#if defined(OS_UNIX_LIKE)
    pid_t id = fork();
    if (CHILD_PROCESS == id){
        shazamc_ffmpeg_start_recording(audio_file_target);
    } else if (PROCESS_INIT_FAILED == id){
        printf("Failed to create child process.\n");
        return 1;
    } else if (CHILD_PROCESS != id){
        sleep(recording_time);
        if (FAILED_TO_SIGTERM == kill(id, SIGTERM)) {
            printf("Could not issue SIGTERM.\n");
        }
        wait(NULL);
        printf("Parent: done.\n");
        printf("hello world ffmpeg! from main process %d\n.", id);
    }
#endif
    return 0;
}



int shazamc_ffmpeg_convert_audio_to_dat(
    const char *audio_file_source, 
    const char *audio_target_dat, 
    const char *recording_time
){
#if defined(OS_UNIX_LIKE)
    pid_t id = fork();
    if (CHILD_PROCESS == id){
        int ret = execlp(
            "ffmpeg", "ffmpeg",
            "-hide_banner",
            "-loglevel", "quiet",
            "-ss", "10", "-t", recording_time,
            "-i", audio_file_source,
            "-ac", "1", "-f", "s16le", "-acodec", "pcm_s16le", "-ar", "44100",
            "-y", audio_target_dat,
            NULL);
        if (SUBPROCESS_FAILED == ret){
            perror("Failed to read audio file.");
            exit(1);
        }
        exit(0);
    } else if (PROCESS_INIT_FAILED == id){
        perror("Failed to create child process.");
        return 1;
    } else {
        wait(NULL);
    }
#endif
    return 0;
}


char* shazamc_ffmpeg_parse_dat_file(const char* dat_file){
    FILE *fptr = fopen(dat_file, "rb"); 
    try_or_return_msg(fptr, NULL, NULL, "Could not open this file."); 

    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    rewind(fptr); 

    if (0 == file_size) return NULL;

    unsigned char *buffer = malloc(file_size);
    if (NULL == buffer) {
        perror("Out of memory.");
        fclose(fptr);
        return NULL;
    }
    size_t read = fread(buffer, 1, file_size, fptr);
    char *result = shazamc_base64_encode(buffer, read);
    fclose(fptr); free(buffer);
    return result;
}