#ifndef FFMPEG_H_
#define FFMPEG_H_



int ffmpeg_record_audio_from_source(
    const char *media_format, 
    const char *audio_source, 
    const char *audio_file_target,
    int recording_time);
int convert_audio_to_dat(){}



#endif /* FFMPEG_H_ */