#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>


#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<fcntl.h>


#include"ffmpeg.h"
#include"utils.h"



#define CHILD_PROCESS                 0
#define AUDIO_BUFFER_SIZE             1024
#define DEFAULT_AUDIO_FILE            "./recording.dat"
#define DEFAULT_AUDIO_B64_FILE        "./recording_b64.dat"


// start_recording() {
//     # should be mono signed 16 bit little endian
//     ffmpeg \
//         -hide_banner \
//         -f avfoundation -i "$AUDIO_SOURCE" \
//         -ac 1 -f s16le -acodec pcm_s16le -ar 44100 \
//         -y "$AUDIO_FILE" 2>&1 &
//     printf "%s\n" "$!" >"$PID_FILE"
// }



int ffmpeg_record_audio_from_source(
    const char *media_format, 
    const char *audio_source, 
    const char *audio_file_target
){
    char filename[] = "sample_fifo";
    if (mkfifo(filename, 0777) == -1){ 
        if (errno != EEXIST) {
            printf("Could not create fifo file.");
            return 1; 
        }
    }
    int id = fork();
    if (id == CHILD_PROCESS){
        string _audio_file_target = init_string(20);
        if (audio_file_target != NULL) {
            append_string(&_audio_file_target, audio_file_target);
        } else {
            append_string(&_audio_file_target, DEFAULT_AUDIO_FILE);
        }
        int fd = open("sample_fifo", O_WRONLY);
        printf("hello world ffmpeg! from child process %d\n.", id);
        fflush(stdout);

        int ret = execlp(
            "ffmpeg", 
            "-hide_banner", 
            "-f", media_format, "-i", audio_source,
            "-ac", "1", "-f", "s16le", "-acodec", "pcm_s16le", "-ar", "44100",
            "-y", _audio_file_target.str,
            NULL);
        close(fd);
        deinit_string(&_audio_file_target);
        if (ret < 0)
        printf("Failed to execute ffmpeg.\n");
        return 1;
    } else if (id < 0){
        printf("Failed to create child process.\n");
        return 1;
    } else if (id != CHILD_PROCESS){
        int fd = open("sample_fifo", O_RDONLY);
        wait(NULL);
        printf("hello world ffmpeg! from main process %d\n.", id);
        close(fd);
    }
    return 0;
}


int ffmpeg_record_audio_from_file(const char *media_format){
    return 0;
}



int recording_ffmpeg(const char *audio_source){
    char buffer[AUDIO_BUFFER_SIZE];
    return 1;
}