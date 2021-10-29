#include "../include/mainwindow.h"
#include "../form/ui_mainwindow.h"

#include <iostream>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->horizontalSliderPlayProgress, &QSlider::valueChanged, this, &MainWindow::setLabelNowValue);
    connect(ui->horizontalSliderVolume, &QSlider::valueChanged, this, &MainWindow::volumeChange);

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
    if (playStatus) {
        playIcon.addFile("../resource/icon/play.png");
        toolTip = "Play";
    } else {
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

void MainWindow::setLabelNowValue(int time) {

    QString now = QTime(0,0,0).addSecs(time).toString(QString::fromStdString("hh:mm:ss"));
    ui->labelNow->setText(now);

}

void MainWindow::volumeChange(int volume) {

    std::cout << volume << std::endl;

}

