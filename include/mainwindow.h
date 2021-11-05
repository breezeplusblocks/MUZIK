#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include "playlistdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pBtnPrev_clicked();

    void on_pBtnPlay_clicked();

    void on_pBtnNext_clicked();

    void on_pBtnPlayMode_clicked();

    void on_pBtnVolume_clicked();

    void on_horizontalSliderPlayProgress_valueChanged(int time);

    void on_horizontalSliderVolume_valueChanged(int volume);

    void on_pBtnPlaylist_clicked();

    void setPlaylistDialogPos();

    void playAudio(QString audioPath);

private:
    Ui::MainWindow *ui;

    PlaylistDialog *plDialog;

    bool playStatus;

    int playMode;

    int musicDuration;

    int volume;

    bool muteStatus;

    QPoint plDialogPos;
    
    QFile musicFile;

    QAudioFormat format;

    QAudioOutput *audio;

    QAudioDeviceInfo info;
};
#endif // MAINWINDOW_H
