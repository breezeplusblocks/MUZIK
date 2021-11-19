#include "../include/mainwindow.h"
#include "../form/ui_mainwindow.h"

#include <iostream>
#include <QTime>
#include <QThread>

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

    connect(plDialog, &PlaylistDialog::pBtnPlayChange, this, &MainWindow::pBtnPlayChange);

    playMode = 0;
    muteStatus = false;

    // set default volume 100
    volume = 100;
    ui->horizontalSliderVolume->setValue(volume);

    // set duration label according to the music info
    musicDuration = 378;
    QString duration = QTime(0,0,0).addSecs(musicDuration).toString(QString::fromStdString("hh:mm:ss"));
    ui->labelDuration->setText(duration);
    ui->horizontalSliderPlayProgress->setMinimum(0);
    ui->horizontalSliderPlayProgress->setMaximum(musicDuration);
    ui->horizontalSliderPlayProgress->setSingleStep(1);

    ffmpeg = new FFmpeg;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pBtnPrev_clicked() {

    QIcon playIcon = ui->pBtnPlay->icon();
    QString toolTip;
    const QString &prevFileName = plDialog->getPrevFileName();
    if (ffmpeg->isRunning() && !prevFileName.isEmpty()) {
        ffmpeg->terminate();
        QThread::msleep(500);
        ffmpeg->init(prevFileName);
        ffmpeg->start();
        playIcon.addFile("../resource/icon/pause.png");
        toolTip = "Pause";
        ui->pBtnPlay->setIcon(playIcon);
        ui->pBtnPlay->setToolTip(toolTip);
    }

    std::cout << prevFileName.toLocal8Bit().data() << std::endl;

}

void MainWindow::on_pBtnPlay_clicked() {

    QIcon playIcon = ui->pBtnPlay->icon();
    QString toolTip;
    const QString &curFileName = plDialog->getFirstOrSelectedFileName();
    if (!ffmpeg->isRunning() && !curFileName.isEmpty()) {
        ffmpeg->init(curFileName);
        ffmpeg->start();
        playIcon.addFile("../resource/icon/pause.png");
        toolTip = "Pause";
    } else {
        ffmpeg->clickPlayBtn(playIcon, toolTip);
    }
    ui->pBtnPlay->setIcon(playIcon);
    ui->pBtnPlay->setToolTip(toolTip);

}

void MainWindow::on_pBtnNext_clicked() {

    QIcon playIcon = ui->pBtnPlay->icon();
    QString toolTip;
    const QString &nextFileName = plDialog->getNextFileName();
    if (ffmpeg->isRunning() && !nextFileName.isEmpty()) {
        ffmpeg->terminate();
        QThread::msleep(500);
        ffmpeg->init(nextFileName);
        ffmpeg->start();
        playIcon.addFile("../resource/icon/pause.png");
        toolTip = "Pause";
        ui->pBtnPlay->setIcon(playIcon);
        ui->pBtnPlay->setToolTip(toolTip);
    }

    std::cout << nextFileName.toLocal8Bit().data() << std::endl;

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

        ffmpeg->setVolume(volume);
    } else {
        volumeIcon.addFile("../resource/icon/mute.png");

        // save the volume slider to variable volume and then set the volume slider to zero.
        volume = ui->horizontalSliderVolume->value();
        ui->horizontalSliderVolume->setValue(0);

        ffmpeg->setVolume(0);
    }
    ui->pBtnVolume->setIcon(volumeIcon);
    muteStatus = !muteStatus;

}

void MainWindow::on_horizontalSliderPlayProgress_valueChanged(int time) {

    QString now = QTime(0,0,0).addSecs(time).toString(QString::fromStdString("hh:mm:ss"));
    ui->labelNow->setText(now);

}

void MainWindow::on_horizontalSliderVolume_valueChanged(int vol) {

    if (ffmpeg) ffmpeg->setVolume(vol);

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
    std::cout << "isRunning = " << ffmpeg->isRunning() << std::endl;
    if (ffmpeg->isRunning()) ffmpeg->terminate();
    QThread::msleep(500);
    ffmpeg->init(audioPath);
    ffmpeg->start();
}

void MainWindow::pBtnPlayChange() {
    QIcon playIcon = ui->pBtnPlay->icon();
    playIcon.addFile("../resource/icon/pause.png");
    QString toolTip = "Pause";
    ui->pBtnPlay->setIcon(playIcon);
    ui->pBtnPlay->setToolTip(toolTip);
}
