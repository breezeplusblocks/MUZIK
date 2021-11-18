#ifndef FFMPEG_H
#define FFMPEG_H
#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QThread>
#include <QtTest/QTest>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

#define MAX_AUDIO_FRAME_SIZE 192000

extern int channelCount;
extern int sampleRate;

enum PlaybackState{
    None, Play, Pause, Resume
};

class FFmpeg : public QThread {
public:

    void init(const QString &audioFilePath);

    void setVolume(int volume);

    void clickPlayBtn(QIcon &playIcon, QString &toolTip);

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

};

#endif //FFMPEG_H
