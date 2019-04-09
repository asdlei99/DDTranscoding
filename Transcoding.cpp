//
//  Transcoding.cpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/2/27.
//  Copyright © 2019 欧江平. All rights reserved.
//

#include "Transcoding.hpp"
#include "WorkContainerProxy.hpp"


static int open_input_file(MediaContext* media_context, const char *filename)
{
    int ret;
    unsigned int i;

    media_context->ifmt_ctx = NULL;
    if ((ret = avformat_open_input(&(media_context->ifmt_ctx), filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(media_context->ifmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }
    media_context->stream_ctx = (StreamContext *) av_mallocz_array(media_context->ifmt_ctx->nb_streams, sizeof(*(media_context->stream_ctx)));
    if (!media_context->stream_ctx)
        return AVERROR(ENOMEM);

    for (i = 0; i < media_context->ifmt_ctx->nb_streams; i++) {
        AVStream *stream = media_context->ifmt_ctx->streams[i];
        AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
        AVCodecContext *codec_ctx;
        if (!dec) {
            av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
            return AVERROR_DECODER_NOT_FOUND;
        }
        codec_ctx = avcodec_alloc_context3(dec);
        if (!codec_ctx) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
            return AVERROR(ENOMEM);
        }
        ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
                    "for stream #%u\n", i);
            return ret;
        }
        /* Reencode video & audio and remux subtitles etc. */
        if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
                || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
                codec_ctx->framerate = av_guess_frame_rate(media_context->ifmt_ctx, stream, NULL);
            /* Open decoder */
            ret = avcodec_open2(codec_ctx, dec, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
                return ret;
            }
        }
        media_context->stream_ctx[i].dec_ctx = codec_ctx;
    }

    av_dump_format(media_context->ifmt_ctx, 0, filename, 0);
    return 0;
}

static int open_output_file(MediaContext* media_context, const char *filename)
{
    printf("fasdafsfasd \n");
    AVStream *out_stream;
    AVStream *in_stream;
    AVCodecContext *dec_ctx, *enc_ctx;
    AVCodec *encoder;
    int ret;
    unsigned int i;

    media_context->video_codec_copy = -1;

    media_context->ofmt_ctx = NULL;
    AVOutputFormat* guss_format = av_guess_format(NULL, filename, NULL);
    const char* format_name = NULL;
    if(guss_format == NULL) {
        if(strstr(filename, "rtmp") != NULL) {
            format_name = "flv";
        }
    }
    avformat_alloc_output_context2(&(media_context->ofmt_ctx), NULL, format_name, filename);
    if (!media_context->ofmt_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
        return AVERROR_UNKNOWN;
    }


    for (i = 0; i < media_context->ifmt_ctx->nb_streams; i++) {
        media_context->stream_ctx[i].audio_fifo = NULL;
        out_stream = avformat_new_stream(media_context->ofmt_ctx, NULL);
        if (!out_stream) {
            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }

        in_stream = media_context->ifmt_ctx->streams[i];
        dec_ctx = media_context->stream_ctx[i].dec_ctx;

        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
                || dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            /* in this example, we choose transcoding to same codec */
            if(dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
                encoder = avcodec_find_encoder(AV_CODEC_ID_MP3);
                //encoder = avcodec_find_encoder(AV_CODEC_ID_AC3);
            } else if(dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
                if(dec_ctx->codec_id == AV_CODEC_ID_H264 && media_context->video_codec_copy != -1) {
                    media_context->video_codec_copy = i;
                }
                //这里encoder继续初始化 但是后面不用此encoder继续编码
                encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
                //encoder = avcodec_find_encoder(AV_CODEC_ID_FLV1);
            } else {
                encoder = avcodec_find_encoder(dec_ctx->codec_id);
            }
            if (!encoder) {
                av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
                return AVERROR_INVALIDDATA;
            }
            enc_ctx = avcodec_alloc_context3(encoder);
            if (!enc_ctx) {
                av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
                return AVERROR(ENOMEM);
            }

            /* In this example, we transcode to same properties (picture size,
             * sample rate etc.). These properties can be changed for output
             * streams easily using filters */

            if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
                out_stream->avg_frame_rate = in_stream->avg_frame_rate;
                out_stream->r_frame_rate = in_stream->r_frame_rate;
                out_stream->sample_aspect_ratio = in_stream->sample_aspect_ratio;
                //out_stream->time_base = in_stream->time_base;

                out_stream->codecpar->codec_tag = 0;

                enc_ctx->height = dec_ctx->height;
                enc_ctx->width = dec_ctx->width;
                enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
                /* take first format from list of supported formats */
                if (encoder->pix_fmts)
                    enc_ctx->pix_fmt = encoder->pix_fmts[0];
                else
                    enc_ctx->pix_fmt = dec_ctx->pix_fmt;
                /* video time_base can be set to whatever is handy and supported by encoder */
                enc_ctx->time_base = av_inv_q(dec_ctx->framerate);
                //enc_ctx->time_base = dec_ctx->time_base;
                enc_ctx->framerate = dec_ctx->framerate;

            } else {
                enc_ctx->sample_rate = dec_ctx->sample_rate;
                enc_ctx->channel_layout = dec_ctx->channel_layout;
                enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
                /* take first format from list of supported formats */
                enc_ctx->sample_fmt = encoder->sample_fmts[0];
                //enc_ctx->sample_fmt = AV_SAMPLE_FMT_S16;
                enc_ctx->time_base = (AVRational){1, enc_ctx->sample_rate};
                media_context->stream_ctx[i].audio_fifo = av_audio_fifo_alloc(enc_ctx->sample_fmt,enc_ctx->channels, 1);
            }

            /* Third parameter can be used to pass settings to encoder */
            ret = avcodec_open2(enc_ctx, encoder, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
                return ret;
            }
            ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
                return ret;
            }
            if (media_context->ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
                //out_stream->time_base = (AVRational){1, 1000};
                out_stream->time_base = enc_ctx->time_base;
            } else {
                out_stream->time_base = enc_ctx->time_base;
            }
            media_context->stream_ctx[i].enc_ctx = enc_ctx;
        } else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
            return AVERROR_INVALIDDATA;
        } else {
            /* if this stream must be remuxed */
            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
                return ret;
            }
            out_stream->time_base = in_stream->time_base;
        }

    }
    av_dump_format(media_context->ofmt_ctx, 0, filename, 1);

    if (!(media_context->ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&(media_context->ofmt_ctx->pb), filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", filename);
            return ret;
        }
    }

    /* init muxer, write output file header */
    ret = avformat_write_header(media_context->ofmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        return ret;
    }

    return 0;
}

static int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx,
        AVCodecContext *enc_ctx, const char *filter_spec)
{
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc = NULL;
    const AVFilter *buffersink = NULL;
    AVFilterContext *buffersrc_ctx = NULL;
    AVFilterContext *buffersink_ctx = NULL;
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVFilterGraph *filter_graph = avfilter_graph_alloc();

    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        buffersrc = avfilter_get_by_name("buffer");
        buffersink = avfilter_get_by_name("buffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                dec_ctx->time_base.num, dec_ctx->time_base.den,
                dec_ctx->sample_aspect_ratio.num,
                dec_ctx->sample_aspect_ratio.den);

        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
            goto end;
        }

        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
                (uint8_t*)&enc_ctx->pix_fmt, sizeof(enc_ctx->pix_fmt),
                AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
            goto end;
        }
    } else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        buffersrc = avfilter_get_by_name("abuffer");
        buffersink = avfilter_get_by_name("abuffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        if (!dec_ctx->channel_layout)
            dec_ctx->channel_layout = av_get_default_channel_layout(dec_ctx->channels);
        snprintf(args, sizeof(args),
                "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
                dec_ctx->time_base.num, dec_ctx->time_base.den, dec_ctx->sample_rate,
                av_get_sample_fmt_name(dec_ctx->sample_fmt),
                dec_ctx->channel_layout);
        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
            goto end;
        }

        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_fmts",
                (uint8_t*)&enc_ctx->sample_fmt, sizeof(enc_ctx->sample_fmt),
                AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "channel_layouts",
                (uint8_t*)&enc_ctx->channel_layout,
                sizeof(enc_ctx->channel_layout), AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_rates",
                (uint8_t*)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
                AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
            goto end;
        }
    } else {
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if (!outputs->name || !inputs->name) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_spec,
                    &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

    /* Fill FilteringContext */
    fctx->buffersrc_ctx = buffersrc_ctx;
    fctx->buffersink_ctx = buffersink_ctx;
    fctx->filter_graph = filter_graph;

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

static int init_filters(MediaContext* media_context)
{
    const char *filter_spec;
    unsigned int i;
    int ret;
    media_context->filter_ctx = (FilteringContext *)av_malloc_array(media_context->ifmt_ctx->nb_streams, sizeof(*(media_context->filter_ctx)));
    if (!media_context->filter_ctx)
        return AVERROR(ENOMEM);

    for (i = 0; i < media_context->ifmt_ctx->nb_streams; i++) {
        media_context->filter_ctx[i].buffersrc_ctx  = NULL;
        media_context->filter_ctx[i].buffersink_ctx = NULL;
        media_context->filter_ctx[i].filter_graph   = NULL;
        if (!(media_context->ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO
                    || media_context->ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO))
            continue;


        if (media_context->ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            filter_spec = "null"; /* passthrough (dummy) filter for video */
        else
            filter_spec = "anull"; /* passthrough (dummy) filter for audio */
        ret = init_filter(&(media_context->filter_ctx[i]), media_context->stream_ctx[i].dec_ctx,
                media_context->stream_ctx[i].enc_ctx, filter_spec);
        if (ret)
            return ret;
    }
    return 0;
}

static int write_packet(MediaContext* media_context, unsigned int stream_index, AVPacket* pkt) {
    int ret;
    av_packet_rescale_ts(pkt,
                         media_context->stream_ctx[stream_index].dec_ctx->time_base,
                         media_context->ofmt_ctx->streams[stream_index]->time_base);
    ret = av_interleaved_write_frame(media_context->ofmt_ctx, pkt);
    return ret;
}

static int encode_write_frame(MediaContext* media_context, AVFrame *filt_frame, unsigned int stream_index, int *got_frame) {
    int ret;
    int got_frame_local;
    AVPacket enc_pkt;
    int (*enc_func)(AVCodecContext *, AVPacket *, const AVFrame *, int *) =
        (media_context->ifmt_ctx->streams[stream_index]->codecpar->codec_type ==
         AVMEDIA_TYPE_VIDEO) ? avcodec_encode_video2 : avcodec_encode_audio2;

    if (!got_frame)
        got_frame = &got_frame_local;

    //av_log(NULL, AV_LOG_INFO, "Encoding frame\n");
    /* encode filtered frame */
    enc_pkt.data = NULL;
    enc_pkt.size = 0;
    av_init_packet(&enc_pkt);
    ret = enc_func(media_context->stream_ctx[stream_index].enc_ctx, &enc_pkt,
            filt_frame, got_frame);
    av_frame_free(&filt_frame);
    if (ret < 0)
        return ret;
    if (!(*got_frame))
        return 0;

    /* prepare packet for muxing */
    enc_pkt.stream_index = stream_index;
    av_packet_rescale_ts(&enc_pkt,
            media_context->stream_ctx[stream_index].enc_ctx->time_base,
            media_context->ofmt_ctx->streams[stream_index]->time_base);

    av_log(NULL, AV_LOG_DEBUG, "Muxing frame\n");
    /* mux encoded frame */
    ret = av_interleaved_write_frame(media_context->ofmt_ctx, &enc_pkt);
    return ret;
}

static int add_samples_to_fifo(AVAudioFifo *fifo,
        uint8_t **converted_input_samples,
        const int frame_size)
{
    int error;

    /* Make the FIFO as large as it needs to be to hold both,
     * the old and the new samples. */
    if ((error = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame_size)) < 0) {
        fprintf(stderr, "Could not reallocate FIFO\n");
        return error;
    }

    /* Store the new samples in the FIFO buffer. */
    if (av_audio_fifo_write(fifo, (void **)converted_input_samples,
                frame_size) < frame_size) {
        fprintf(stderr, "Could not write data to FIFO\n");
        return AVERROR_EXIT;
    }
    return 0;
}

static int filter_encode_write_frame(MediaContext* media_context, AVFrame *frame, unsigned int stream_index)
{
    int ret;
    AVFrame *filt_frame;

    //av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
    /* push the decoded frame into the filtergraph */
    ret = av_buffersrc_add_frame_flags(media_context->filter_ctx[stream_index].buffersrc_ctx,
            frame, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        return ret;
    }

    /* pull filtered frames from the filtergraph */
    while (1) {
        filt_frame = av_frame_alloc();
        if (!filt_frame) {
            ret = AVERROR(ENOMEM);
            break;
        }
        //av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
        ret = av_buffersink_get_frame(media_context->filter_ctx[stream_index].buffersink_ctx,
                filt_frame);
        if (ret < 0) {
            /* if no more frames for output - returns AVERROR(EAGAIN)
             * if flushed and no more frames for output - returns AVERROR_EOF
             * rewrite retcode to 0 to show it as normal procedure completion
             */
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                ret = 0;
            av_frame_free(&filt_frame);
            break;
        }

        filt_frame->pict_type = AV_PICTURE_TYPE_NONE;

        if(media_context->ifmt_ctx->streams[stream_index]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            AVAudioFifo* fifo = media_context->stream_ctx[stream_index].audio_fifo;

            add_samples_to_fifo(fifo, filt_frame->data, filt_frame->nb_samples);

            int encoder_frame_size = media_context->stream_ctx[stream_index].enc_ctx->frame_size;
            //printf("fifo %d %d %d\n", av_audio_fifo_size(fifo), filt_frame->nb_samples, encoder_frame_size);

            while(av_audio_fifo_size(fifo) >= encoder_frame_size) {

                AVFrame* encode_frame;
                int error = 0;

                if((encode_frame = av_frame_alloc())) {
                    encode_frame->nb_samples = encoder_frame_size;
                    encode_frame->channel_layout = filt_frame->channel_layout;
                    encode_frame->format = filt_frame->format;
                    encode_frame->sample_rate = filt_frame->sample_rate;
                    encode_frame->pts = filt_frame->pts;
                    encode_frame->pkt_pts = filt_frame->pkt_pts;
                    encode_frame->pkt_dts = filt_frame->pkt_dts;

                    if ((error = av_frame_get_buffer(encode_frame, 0)) < 0) {
                        fprintf(stderr, "Could not allocate output frame samples (error '%s')\n",
                                av_err2str(error));
                        av_frame_free(&encode_frame);
                        break;
                    }

                    if (av_audio_fifo_read(fifo, (void **)encode_frame->data, encoder_frame_size) < encoder_frame_size) {
                        fprintf(stderr, "Could not read data from FIFO\n");
                        av_frame_free(&encode_frame);
                        break;
                    }
                    ret = encode_write_frame(media_context, encode_frame, stream_index, NULL);
                }
            }

            av_frame_free(&filt_frame);
        } else {
            ret = encode_write_frame(media_context, filt_frame, stream_index, NULL);
            if (ret < 0)
                break;
        }
    }

    return ret;
}

static int flush_encoder(MediaContext* media_context, unsigned int stream_index)
{
    int ret;
    int got_frame;

    if (!(media_context->stream_ctx[stream_index].enc_ctx->codec->capabilities &
                AV_CODEC_CAP_DELAY))
        return 0;

    while (1) {
        av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
        ret = encode_write_frame(media_context, NULL, stream_index, &got_frame);
        if (ret < 0)
            break;
        if (!got_frame)
            return 0;
    }
    return ret;
}

int transcoding(const char* src, const char* dst, ControlContex* control_contex)
{
    MediaContext* media_context = (MediaContext*)malloc(sizeof(MediaContext));
    int ret;
    AVPacket packet = { .data = NULL, .size = 0 };
    AVFrame *frame = NULL;
    enum AVMediaType type;
    unsigned int stream_index;
    unsigned int i;
    int got_frame;
    int (*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket *);

    if ((ret = open_input_file(media_context, src)) < 0)
        goto end;
    if ((ret = open_output_file(media_context, dst)) < 0)
        goto end;

    if ((ret = init_filters(media_context)) < 0)
        goto end;


    /* read all packets */
    while (1 && ((control_contex->running_flag != RUNNING_FLAG_STOPPING) && (control_contex->running_flag != RUNNING_FLAG_STOPED))) {
        control_contex->running_flag = RUNNING_FLAG_RUNNING;
        if ((ret = av_read_frame(media_context->ifmt_ctx, &packet)) < 0)
            break;

        stream_index = packet.stream_index;
        type = media_context->ifmt_ctx->streams[packet.stream_index]->codecpar->codec_type;
        av_log(NULL, AV_LOG_DEBUG, "Demuxer gave frame of stream_index %u\n",
                stream_index);

        if (media_context->filter_ctx[stream_index].filter_graph) {
            av_log(NULL, AV_LOG_DEBUG, "Going to reencode&filter the frame\n");
            frame = av_frame_alloc();
            if (!frame) {
                ret = AVERROR(ENOMEM);
                break;
            }
            av_packet_rescale_ts(&packet,
                    media_context->ifmt_ctx->streams[stream_index]->time_base,
                    media_context->stream_ctx[stream_index].dec_ctx->time_base);

            dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 :
                avcodec_decode_audio4;


            if(type == AVMEDIA_TYPE_VIDEO) {
                //media_context->stream_ctx[stream_index].enc_ctx->time_base.num = media_context->stream_ctx[stream_index].dec_ctx->time_base.num;
                //media_context->stream_ctx[stream_index].enc_ctx->time_base.den = media_context->stream_ctx[stream_index].dec_ctx->time_base.den;
                media_context->stream_ctx[stream_index].enc_ctx->time_base = media_context->stream_ctx[stream_index].dec_ctx->time_base;
            }

            ret = dec_func(media_context->stream_ctx[stream_index].dec_ctx, frame,
                    &got_frame, &packet);
            if (ret < 0) {
                av_frame_free(&frame);
                av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
                continue;
                //break;
            }

            if (got_frame) {
                frame->pts = frame->best_effort_timestamp;
                if(type == AVMEDIA_TYPE_VIDEO && media_context->video_codec_copy == stream_index) {
                    //ret = encode_write_frame(media_context, frame, stream_index, NULL);
                    ret = write_packet(media_context, stream_index, &packet);
                } else {
                    ret = filter_encode_write_frame(media_context, frame, stream_index);
                    av_frame_free(&frame);
                }
                if (ret < 0) {
                    continue;
                    //goto end;
                }
            } else {
                av_frame_free(&frame);
            }
        } else {
            /* remux this frame without reencoding */
            av_packet_rescale_ts(&packet,
                    media_context->ifmt_ctx->streams[stream_index]->time_base,
                    media_context->ofmt_ctx->streams[stream_index]->time_base);

            ret = av_interleaved_write_frame(media_context->ofmt_ctx, &packet);
            if (ret < 0)
                goto end;
        }
        av_packet_unref(&packet);
    }


    /* flush filters and encoders */
    for (i = 0; i < media_context->ifmt_ctx->nb_streams; i++) {
        /* flush filter */
        if (!media_context->filter_ctx[i].filter_graph)
            continue;
        ret = filter_encode_write_frame(media_context, NULL, i);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Flushing filter failed\n");
            goto end;
        }

        /* flush encoder */
        ret = flush_encoder(media_context, i);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Flushing encoder failed\n");
            goto end;
        }
    }

    av_write_trailer(media_context->ofmt_ctx);
end:
    av_packet_unref(&packet);
    //av_frame_free(&frame);
    for (i = 0; i < media_context->ifmt_ctx->nb_streams; i++) {
        if(media_context->stream_ctx[i].audio_fifo) {
            av_audio_fifo_free(media_context->stream_ctx[i].audio_fifo);
        }
        avcodec_free_context(&(media_context->stream_ctx[i].dec_ctx));
        if (media_context->ofmt_ctx && media_context->ofmt_ctx->nb_streams > i && media_context->ofmt_ctx->streams[i] && media_context->stream_ctx[i].enc_ctx)
            avcodec_free_context(&(media_context->stream_ctx[i].enc_ctx));
        if (media_context->filter_ctx && media_context->filter_ctx[i].filter_graph)
            avfilter_graph_free(&(media_context->filter_ctx[i].filter_graph));
    }
    av_free(media_context->filter_ctx);
    av_free(media_context->stream_ctx);
    avformat_close_input(&(media_context->ifmt_ctx));
    if (media_context->ofmt_ctx && !(media_context->ofmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&(media_context->ofmt_ctx->pb));
    avformat_free_context(media_context->ofmt_ctx);
    free(media_context);

    if (ret < 0)
        av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n", av_err2str(ret));

    control_contex->running_flag = RUNNING_FLAG_STOPED;
    WorkContainerProxy* workerCnntainerProxy = WorkContainerProxy::getWorkContainerProxy();
    workerCnntainerProxy->clearWork(dst);
    return ret ? 1 : 0;
}
