#include "../include/mainwindow.h"
#include "../form/ui_mainwindow.h"

#include <iostream>
#include <QTime>

int channelCount;
int sampleRate;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // initialize playlist dialog
    plDialog = new PlaylistDialog(this);

    connect(plDialog, &QDialog::rejected, this, &MainWindow::setPlaylistDialogPos);

    connect(plDialog, &PlaylistDialog::playAudio, this, &MainWindow::playAudio);

    playStatus = false;
    playMode = 0;
    muteStatus = false;

    // set default volume 50
    volume = 50;
    ui->horizontalSliderVolume->setValue(volume);

    // set duration label according to the music info
    musicDuration = 378;
    QString duration = QTime(0,0,0).addSecs(musicDuration).toString(QString::fromStdString("hh:mm:ss"));
    ui->labelDuration->setText(duration);
    ui->horizontalSliderPlayProgress->setMinimum(0);
    ui->horizontalSliderPlayProgress->setMaximum(musicDuration);
    ui->horizontalSliderPlayProgress->setSingleStep(1);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pBtnPrev_clicked() {

    std::cout << "Previous" << std::endl;

}

void MainWindow::on_pBtnPlay_clicked() {

    QIcon playIcon = ui->pBtnPlay->icon();
    QString toolTip;
    if (audioOutput->state() == QAudio::ActiveState) {
        audioOutput->suspend();
        playIcon.addFile("../resource/icon/play.png");
        toolTip = "Play";
    } else {
        audioOutput->resume();
        playIcon.addFile("../resource/icon/pause.png");
        toolTip = "Pause";
    }
    ui->pBtnPlay->setIcon(playIcon);
    ui->pBtnPlay->setToolTip(toolTip);
    playStatus = !playStatus;

}

void MainWindow::on_pBtnNext_clicked() {

    std::cout << "Next" << std ::endl;

}

void MainWindow::on_pBtnPlayMode_clicked() {

    QIcon playModeIcon = ui->pBtnPlayMode->icon();
    QString toolTip;
    if (0 == playMode) {
        playModeIcon.addFile("../resource/icon/shuffle.png");
        toolTip = "Shuffle";
        ++playMode;
    } else if (1 == playMode) {
        playModeIcon.addFile("../resource/icon/single.png");
        toolTip = "Single";
        ++playMode;
    } else if (2 == playMode) {
        playModeIcon.addFile("../resource/icon/repeat.png");
        toolTip = "Repeat";
        ++playMode;
    } else if (3 == playMode) {
        playModeIcon.addFile("../resource/icon/order.png");
        toolTip = "Order";
        playMode = 0;
    }
    ui->pBtnPlayMode->setIcon(playModeIcon);
    ui->pBtnPlayMode->setToolTip(toolTip);

}

void MainWindow::on_pBtnVolume_clicked() {

    QIcon volumeIcon = ui->pBtnVolume->icon();
    if (muteStatus) {
        volumeIcon.addFile("../resource/icon/volume.png");

        // set the volume slider to the value of the variable volume
        ui->horizontalSliderVolume->setValue(volume);
    } else {
        volumeIcon.addFile("../resource/icon/mute.png");

        // save the volume slider to variable volume and then set the volume slider to zero.
        volume = ui->horizontalSliderVolume->value();
        ui->horizontalSliderVolume->setValue(0);
    }
    ui->pBtnVolume->setIcon(volumeIcon);
    muteStatus = !muteStatus;

}

void MainWindow::on_horizontalSliderPlayProgress_valueChanged(int time) {

    QString now = QTime(0,0,0).addSecs(time).toString(QString::fromStdString("hh:mm:ss"));
    ui->labelNow->setText(now);

}

void MainWindow::on_horizontalSliderVolume_valueChanged(int volume) {

    std::cout << volume << std::endl;

}

void MainWindow::on_pBtnPlaylist_clicked() {

    if (plDialog->isHidden()) {
        plDialog->move(plDialogPos);
        plDialog->show();
    } else {
        plDialogPos = plDialog->pos();
        plDialog->hide();
    }

}

void MainWindow::setPlaylistDialogPos() {

    plDialogPos = plDialog->pos();

}

void MainWindow::playAudio(const QString& audioPath) {

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
    while (0 <= av_read_frame(pAVFmtCtx, pkt)) {
        if (audioStreamIdx == pkt->stream_index) {

            // Decode
            if (0 <= avcodec_send_packet(pAVCodecCtx, pkt)) {

                // Receive frame
                while (0 <= avcodec_receive_frame(pAVCodecCtx, frame)) {
                    int len = swr_convert(swrCtx, &audio_out_buffer, MAX_AUDIO_FRAME_SIZE * 2, (const uint8_t **) frame->data, frame->nb_samples);
                    if (0 > len) continue;
                    int out_size = av_samples_get_buffer_size(nullptr, out_channels, len, out_sample_fmt, 1);
                    sleepTime = (out_sample_rate * 16 * out_channels / 8) / out_size;
                    if (out_size > audioOutput->bytesFree()) QTest::qSleep(sleepTime);
                    streamOut->write((char *)audio_out_buffer, out_size);
                }
            }
        }
        av_packet_unref(pkt);
    }

    // Free all
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_close(pAVCodecCtx);
    avcodec_free_context(&pAVCodecCtx);
    avformat_free_context(pAVFmtCtx);

}

