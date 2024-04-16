//
// Created by MMM on 2024/4/16.
//

#include "include/FFmpegPlayAudio.h"

// engine interfaces
static SLObjectItf engineObject = NULL;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
        SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;


int FFmpegPlayAudio::playAudio(const char *inputUrl) {
    int rate, channel;
    LOGD("...get url=%s", inputUrl);
    // 1、初始化FFmpeg解码器
    initFFmpeg(&rate, &channel, inputUrl);

    // 2、初始化OpenSLES
//    initOpenSLES();

    // 3、初始化BufferQueue
//    initBufferQueue(rate, channel, SL_PCMSAMPLEFORMAT_FIXED_16);

    // 4、启动音频播放
//    bqPlayerCallback(bqPlayerBufferQueue, NULL);
    return 0;
}

void FFmpegPlayAudio::stopAudio() {
    // destroy buffer queue audio player object, and invalidate all associated interfaces
   /* if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
        bqPlayerMuteSolo = NULL;
        bqPlayerVolume = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    // 释放FFmpeg解码器
    releaseFFmpeg();*/

}

int FFmpegPlayAudio::initFFmpeg(int *rate, int *channel, const char *url) {
//    av_register_all();
    aFormatCtx = avformat_alloc_context();
    LOGD("ffmpeg get url=:%s", url);
    // 网络音频流
    const char *file_name = url;

    // Open audio file
    if (avformat_open_input(&aFormatCtx, file_name, NULL, NULL) != 0) {
        LOGE("Couldn't open file:%s\n", file_name);
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if (avformat_find_stream_info(aFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.");
        return -1;
    }

    // Find the first audio stream
    int i;
    audioStream = -1;
    for (i = 0; i < aFormatCtx->nb_streams; i++) {
        if (aFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO &&
            audioStream < 0) {
            audioStream = i;
        }
    }
    if (audioStream == -1) {
        LOGE("Couldn't find audio stream!");
        return -1;
    }

    // Get a pointer to the codec context for the video stream
    aCodecCtx = aFormatCtx->streams[audioStream]->codec;

    // Find the decoder for the audio stream
    AVCodec *aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
    if (!aCodec) {
        fprintf(stderr, "Unsupported codec!\n");
        return -1;
    }

    if (avcodec_open2(aCodecCtx, aCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }

    aFrame = av_frame_alloc();

    // 设置格式转换
    swr = swr_alloc();
    av_opt_set_int(swr, "in_channel_layout", aCodecCtx->channel_layout, 0);
    av_opt_set_int(swr, "out_channel_layout", aCodecCtx->channel_layout, 0);
    av_opt_set_int(swr, "in_sample_rate", aCodecCtx->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", aCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", aCodecCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr);

    // 分配PCM数据缓存
    outputBufferSize = 8196;
    outputBuffer = (uint8_t *) malloc(sizeof(uint8_t) * outputBufferSize);

    // 返回sample rate和channels
    *rate = aCodecCtx->sample_rate;
    *channel = aCodecCtx->channels;
    return 0;
}

void FFmpegPlayAudio::initOpenSLES() {

/*    LOGD(">> initOpenSLES...");
    SLresult result;

    // 1、create engine
    result = slCreateEngine(&engineObject
            , 0, NULL
            , 0, NULL, NULL);
    LOGD(">> initOpenSLES... step 1, result = %d", result);

    // 2、realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    LOGD(">> initOpenSLES...step 2, result = %d", result);

    // 3、get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    LOGD(">> initOpenSLES...step 3, result = %d", result);

    // 4、create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);
    LOGD(">> initOpenSLES...step 4, result = %d", result);

    // 5、realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    LOGD(">> initOpenSLES...step 5, result = %d", result);

    // 6、get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        LOGD(">> initOpenSLES...step 6, result = %d", result);
    }*/

}

/*void FFmpegPlayAudio::initBufferQueue(int rate, int channel, int bitsPerSample) {

    LOGD(">> initBufferQueue");
    SLresult result;

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm;
    format_pcm.formatType = SL_DATAFORMAT_PCM;
    format_pcm.numChannels = channel;
    format_pcm.samplesPerSec = rate * 1000;
    format_pcm.bitsPerSample = bitsPerSample;
    format_pcm.containerSize = 16;
    if (channel == 2)
        format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    else
        format_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
    format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            *//*SL_IID_MUTESOLO,*//* SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            *//*SL_BOOLEAN_TRUE,*//* SL_BOOLEAN_TRUE};

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
                                                3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
//    (void) result;

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the effect send interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
                                             &bqPlayerEffectSend);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;
}*/

/*void FFmpegPlayAudio::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {

    LOGD(">> buffere queue callback");
    assert(bq == bqPlayerBufferQueue);
    bufferSize = 0;
    //assert(NULL == context);
//    getPCM(&buffer, &bufferSize);
    // for streaming playback, replace this test by logic to find and fill the next buffer
    if (NULL != buffer && 0 != bufferSize) {
        SLresult result;
        // enqueue another buffer
        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buffer,
                                                 bufferSize);
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
        assert(SL_RESULT_SUCCESS == result);

        (void) result;
    }
}*/

/*// 获取PCM数据, 自动回调获取
int FFmpegPlayAudio::getPCM(void **pcm, size_t *pcmSize) {
    LOGD(">> getPcm");
    while (av_read_frame(aFormatCtx, &packet) >= 0) {

        int frameFinished = 0;
        // Is this a packet from the audio stream?
        if (packet.stream_index == audioStream) {
            avcodec_decode_audio4(aCodecCtx, aFrame, &frameFinished, &packet);

            if (frameFinished) {
                // data_size为音频数据所占的字节数
                int data_size = av_samples_get_buffer_size(
                        aFrame->linesize, aCodecCtx->channels,
                        aFrame->nb_samples, aCodecCtx->sample_fmt, 1);
                LOGD(">> getPcm data_size=%d", data_size);
                // 这里内存再分配可能存在问题
                if (data_size > outputBufferSize) {
                    outputBufferSize = data_size;
                    outputBuffer = (uint8_t *) realloc(outputBuffer,
                                                       sizeof(uint8_t) * outputBufferSize);
                }

                // 音频格式转换
                swr_convert(swr, &outputBuffer, aFrame->nb_samples,
                            (uint8_t const **) (aFrame->extended_data),
                            aFrame->nb_samples);

                // 返回pcm数据
                *pcm = outputBuffer;
                *pcmSize = data_size;
                return 0;
            }
        }
    }
    return -1;
}*/

int FFmpegPlayAudio::releaseFFmpeg() {
    av_packet_unref(&packet);
    av_free(outputBuffer);
    av_free(aFrame);
    avcodec_close(aCodecCtx);
    avformat_close_input(&aFormatCtx);
    return 0;
}


