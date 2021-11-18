#include "../include/ffmpeg.h"
#include <iostream>

void FFmpeg::run() {
    this->playAudio();
}

void FFmpeg::init(const QString &audioFilePath) {
    this->audioPath = audioFilePath;
}

void FFmpeg::setVolume(int vol) {
    if (this->audioOutput) {
//        volume = QAudio::convertVolume(vol / qreal(100.0), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
        volume = vol / qreal(100.0);
//        std::cout << "volume = " << volume << std::endl;
        audioOutput->setVolume(volume);
    }
}

void FFmpeg::play() {

}

void FFmpeg::pause() {
    if (this->isRunning()) {
        state = Pause;
    }
}

void FFmpeg::resume() {
    if (this->isRunning()) {
        state = Resume;
    }
}

bool FFmpeg::playControl() {
    bool res = false;
    if (state == Pause) {
        while (state == Pause) {
            std::cout << "state = " << state << std::endl;
            audioOutput->suspend();
            msleep(500);
        }

        if (state == Resume) audioOutput->resume();
    } else if (state == Play) {
        res = true;
        if (audioOutput->state() == QAudio::ActiveState)
            audioOutput->stop();
    }
    return res;
}

void FFmpeg::playAudio() {

    // Init audio stream index
    int audioStreamIdx = -1;

    AVFormatContext *pAVFmtCtx = nullptr;
    // Open an input stream and read the header.
    // The codecs are not opened. The stream must be closed with avformat_close_input().
    int res = avformat_open_input(&pAVFmtCtx, audioPath.toLocal8Bit().data(), nullptr, nullptr);
    if (0 != res) {
        std::cout << "Open" << audioPath.toLocal8Bit().data() << "failed!" << std::endl;
        exit(-1);
    }

    // Get stream info
    res = avformat_find_stream_info(pAVFmtCtx, nullptr);
    if (0 > res) {
        std::cout << "Get avformat stream info failed!" << std::endl;
        exit(-1);
    }

    // Get audio file duration
    musicDuration = pAVFmtCtx->duration / AV_TIME_BASE;

    // Setup audioStreamIdx if there is an audio stream in opened file
    for (unsigned i = 0; i < pAVFmtCtx->nb_streams; ++i) {
        AVStream *pAVStream = pAVFmtCtx->streams[i];
        if (AVMEDIA_TYPE_AUDIO == pAVStream->codecpar->codec_type) {
            audioStreamIdx = i;
            sampleRate = pAVStream->codecpar->sample_rate;
            channelCount = pAVStream->codecpar->channels;
        }
    }

    if (-1 == audioStreamIdx) {
        std::cout << "Cannot find audio stream!" << std::endl;
        exit(-1);
    }

    // Setup QAudioFormat parameters
    audioFmt.setSampleRate(sampleRate);
    audioFmt.setChannelCount(channelCount);
    audioFmt.setSampleSize(16);
    audioFmt.setCodec("audio/pcm");
    audioFmt.setByteOrder(QAudioFormat::LittleEndian);
    audioFmt.setSampleType(QAudioFormat::SignedInt);

    // If the default output device does not support the audio format, exit the program
    info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(audioFmt)) {
        std::cout << "Audio format not supported by backend. Program will exit." << std::endl;
        exit(-1);
    }

    audioOutput = new QAudioOutput(audioFmt);
    streamOut = audioOutput->start();

    // Find corresponding decoder for the audio stream
    AVCodecParameters *pAVCodecParams = pAVFmtCtx->streams[audioStreamIdx]->codecpar;
    AVCodec *pAVCodec = avcodec_find_decoder(pAVCodecParams->codec_id);
    if (!pAVCodec) {
        std::cout << "Cannot find the codec!" << std::endl;
        exit(-1);
    }

    // Allocate an AVCodecContext and set its fields to default values.
    // The resulting struct should be freed with avcodec_free_context().
    AVCodecContext *pAVCodecCtx = avcodec_alloc_context3(pAVCodec);
    res = avcodec_parameters_to_context(pAVCodecCtx, pAVCodecParams);
    if (0 > res) {
        std::cout << "Cannot alloc codec context." << std::endl;
        exit(-1);
    }
    pAVCodecCtx->pkt_timebase = pAVFmtCtx->streams[audioStreamIdx]->time_base;
//    pAVCodecCtx->thread_count = 8; // 8 thread to decode in the meanwhile

    // Open audio codec
    res = avcodec_open2(pAVCodecCtx, pAVCodec, nullptr);
    if (0 > res) {
        std::cout << "Cannot open audio codec." << std::endl;
        exit(-1);
    }

    // Setup decode params
    // If AVCodecContext's channel_layout is zero
    if (!pAVCodecCtx->channel_layout) {
        if (1 == pAVCodecParams->channels)
            pAVCodecCtx->channel_layout = AV_CH_LAYOUT_MONO;
        else if (2 == pAVCodecParams->channels)
            pAVCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
    }
    uint64_t out_channel_layout = pAVCodecCtx->channel_layout;
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    int out_sample_rate = pAVCodecCtx->sample_rate;
    int out_channels = pAVCodecCtx->channels;

    uint8_t *audio_out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE * 2);

    SwrContext *swrCtx = swr_alloc_set_opts(nullptr,
                                            out_channel_layout,
                                            out_sample_fmt,
                                            out_sample_rate,
                                            pAVCodecCtx->channel_layout,
                                            pAVCodecCtx->sample_fmt,
                                            pAVCodecCtx->sample_rate,
                                            0,
                                            nullptr);
    swr_init(swrCtx);

    double sleepTime = 0;

    // Use Packet & Frame to decode
    AVPacket *pkt = av_packet_alloc(); // freed using av_packet_free().
    AVFrame *frame = av_frame_alloc(); // freed using av_frame_free().
    while (true) {

        if (playControl()) break;

        while (0 <= av_read_frame(pAVFmtCtx, pkt)) {
            if (audioStreamIdx == pkt->stream_index) {

                // Decode
                if (0 <= avcodec_send_packet(pAVCodecCtx, pkt)) {

                    // Receive frame
                    while (0 <= avcodec_receive_frame(pAVCodecCtx, frame)) {

                        if (playControl()) break;

                        int len = swr_convert(swrCtx, &audio_out_buffer, MAX_AUDIO_FRAME_SIZE * 2,
                                              (const uint8_t **) frame->data, frame->nb_samples);
                        if (0 > len) continue;
                        int out_size = av_samples_get_buffer_size(nullptr, out_channels, len, out_sample_fmt, 1);
                        sleepTime = (out_sample_rate * 16 * out_channels / 8) / out_size;
                        if (out_size > audioOutput->bytesFree()) {
                            if (playControl()) break;
                            msleep(sleepTime);
                        }
                    std::cout << "volume = " << this->volume << std::endl;
                        if (!playControl())
                            streamOut->write((char *) audio_out_buffer, out_size);
//                    std::cout << "QAudio State = " << audioOutput->error() << std::endl;
                    }
                }
            }
            av_packet_unref(pkt);
        }
    }

    // Free all
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_close(pAVCodecCtx);
    avcodec_free_context(&pAVCodecCtx);
    avformat_free_context(pAVFmtCtx);

}

void FFmpeg::clickPlayBtn(QIcon &playIcon, QString &toolTip) {
    if (audioOutput->state() == QAudio::ActiveState) {
        this->pause();
        playIcon.addFile("../resource/icon/play.png");
        toolTip = "Play";
    } else {
        this->resume();
        playIcon.addFile("../resource/icon/pause.png");
        toolTip = "Pause";
    }
}
