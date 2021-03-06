#ifndef FFMPEG_H
#define FFMPEG_H
#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QThread>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QtTest/QTest>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#define MAX_AUDIO_FRAME_SIZE 192000

extern int channelCount;
extern int sampleRate;

enum PlaybackState {
    None, Play, Pause, Resume, Stop, Seek
};

enum PlayMode {
    PLAY_MODE_ORDER,
    PLAY_MODE_SHUFFLE,
    PLAY_MODE_SINGLE,
    PLAY_MODE_REPEAT
};

class FFmpeg : public QThread {
public:

    void init(const QString &audioFilePath, QListWidget *qListWidget, QListWidgetItem *item, QPushButton *pBtnPlay, QSlider *timeSlider, QLabel *durationLabel, QLabel *nowLabel);

    void setVolume(int volume);

    void setPlayMode(enum PlayMode mode);

    void clickPlayBtn(QIcon &playIcon, QString &toolTip);

    int getNextRowNum(int cur, int total, bool next);

    void sliderValueChange(int curSec);

private:

    void run() override;

    void playAudio();

    void play();

    void pause();

    void resume();

    bool playControl();

    int musicDuration;

    PlaybackState state;

    qreal volume = 1.0;

    QString audioPath;

    QAudioFormat audioFmt;

    QAudioOutput *audioOutput;

    QIODevice *streamOut;

    QAudioDeviceInfo info;

    QListWidget *listWidget;

    QListWidgetItem *listWidgetItem;

    QPushButton *playButton;

    QSlider *sliderPlayProgress;

    QLabel *labelDuration;

    QLabel *labelNow;

    enum PlayMode playMode = PLAY_MODE_ORDER;

    int curDuration = 0;

    int audioStreamIdx = -1;

    AVFormatContext *pAVFmtCtx = nullptr;

    AVStream *pAVStream;

};

#endif //FFMPEG_H
