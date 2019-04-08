//
//  Transcoding.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/2/27.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef Transcoding_hpp
#define Transcoding_hpp

#include <stdio.h>
//#include <pthread.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/audio_fifo.h>
}


//static AVFormatContext *ifmt_ctx;
//static AVFormatContext *ofmt_ctx;
typedef struct FilteringContext {
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
} FilteringContext;
//static FilteringContext *filter_ctx;

typedef struct StreamContext {
    AVCodecContext *dec_ctx;
    AVCodecContext *enc_ctx;
    AVAudioFifo *audio_fifo;
} StreamContext;
//static StreamContext *stream_ctx;

typedef struct MediaContext {
    StreamContext *stream_ctx;
    FilteringContext *filter_ctx;
    AVFormatContext *ifmt_ctx;
    AVFormatContext *ofmt_ctx;
    int video_codec_copy;
} MediaContext;

enum RUNNING_FLAG {
    RUNNING_FLAG_PREPARE = 0,
    RUNNING_FLAG_RUNNING,
    RUNNING_FLAG_STOPPING,
    RUNNING_FLAG_STOPED
};

typedef struct ControlContex {
    RUNNING_FLAG running_flag;
    //pthread_mutex_t* mute;
} ControlContex;

static int get_running_flag(ControlContex* control_context) {
    //pthread_mutex_lock(control_context->mute);
    enum RUNNING_FLAG flag = control_context->running_flag;
    //pthread_mutex_unlock(control_context->mute);
    return flag;
}

static void set_running_flag(ControlContex* control_context, RUNNING_FLAG flag) {
    //pthread_mutex_lock(control_context->mute);
    control_context->running_flag = flag;
    //pthread_mutex_unlock(control_context->mute);
}

static int open_input_file(MediaContext* media_context, const char *filename);

static int open_output_file(MediaContext* media_context, const char *filename);

static int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx,
        AVCodecContext *enc_ctx, const char *filter_spec);

static int init_filters(MediaContext* media_context);

static int encode_write_frame(MediaContext* media_context, AVFrame *filt_frame, unsigned int stream_index, int *got_frame);

static int write_packet(MediaContext* media_context, unsigned int stream_index, AVPacket* pkt);

static int add_samples_to_fifo(AVAudioFifo *fifo,
        uint8_t **converted_input_samples,
        const int frame_size);

static int filter_encode_write_frame(MediaContext* media_context, AVFrame *frame, unsigned int stream_index);

static int flush_encoder(MediaContext* media_context, unsigned int stream_index);

int transcoding(const char* src, const char* dst, ControlContex* control_context);

#endif
