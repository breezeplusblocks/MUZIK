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

    playMode = PLAY_MODE_ORDER;
    muteStatus = false;

    // set default volume 100
    volume = 100;
    ui->horizontalSliderVolume->setValue(volume);

    // set duration label according to the music info
    ui->horizontalSliderPlayProgress->setMinimum(0);
    ui->horizontalSliderPlayProgress->setSingleStep(1);

    // disable the play progress slider
    ui->horizontalSliderPlayProgress->setDisabled(true);

    ffmpeg = new FFmpeg;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pBtnPrev_clicked() {

    QIcon playIcon = ui->pBtnPlay->icon();
    QString prevFileName;
    QListWidget *listWidget = plDialog->getQListWidget();
    QListWidgetItem *currentItem;
    if (listWidget && listWidget->count() > 1) {
        currentItem = listWidget->currentItem();
        if (currentItem) {
            int curRow = listWidget->row(currentItem);
            if (PLAY_MODE_ORDER == this->playMode && 0 == curRow) {
            } else {
                int total = listWidget->count();
                int row = ffmpeg->getNextRowNum(curRow, total, false);
                currentItem = listWidget->item(row);
                currentItem->setSelected(true);
                listWidget->setCurrentItem(currentItem);
                prevFileName = currentItem->text();
            }
        }
    }
    if (!prevFileName.isEmpty()) {
        if (ffmpeg->isRunning()) {
            ffmpeg->terminate();
            QThread::msleep(500);
        }
        ffmpeg->init(prevFileName, listWidget, currentItem, this->ui->pBtnPlay, ui->horizontalSliderPlayProgress, ui->labelDuration, ui->labelNow);
        ffmpeg->start();
        playIcon.addFile("../resource/icon/pause.png");
        QString toolTip = "Pause";
        ui->pBtnPlay->setIcon(playIcon);
        ui->pBtnPlay->setToolTip(toolTip);
    }

    std::cout << prevFileName.toLocal8Bit().data() << std::endl;

}

void MainWindow::on_pBtnPlay_clicked() {

    QIcon playIcon = ui->pBtnPlay->icon();
    QString toolTip;
    QString curFileName;
    QListWidget *listWidget = plDialog->getQListWidget();
    QListWidgetItem *currentItem;
    if (listWidget) {
        currentItem = listWidget->currentItem();
        if (currentItem) {
            curFileName = currentItem->text();
        } else {
            currentItem = listWidget->item(0);
            currentItem->setSelected(true);
            listWidget->setCurrentItem(currentItem);
            curFileName = currentItem->text();
        }
    }
    if (!ffmpeg->isRunning() && !curFileName.isEmpty()) {
        ffmpeg->init(curFileName, listWidget, currentItem, this->ui->pBtnPlay, ui->horizontalSliderPlayProgress, ui->labelDuration, ui->labelNow);
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
    QString nextFileName;
    QListWidget *listWidget = plDialog->getQListWidget();
    QListWidgetItem *currentItem;
    if (listWidget) {
        int count = listWidget->count();
        if (count > 1) {
            currentItem = listWidget->currentItem();
            if (currentItem) {
                int curRow = listWidget->row(currentItem);
                if (PLAY_MODE_ORDER == this->playMode && curRow + 1 == count) {
                } else {
                    int total = listWidget->count();
                    int row = ffmpeg->getNextRowNum(curRow, total, true);
                    currentItem = listWidget->item(row);
                    currentItem->setSelected(true);
                    listWidget->setCurrentItem(currentItem);
                    nextFileName = currentItem->text();
                }
            }
        }
    }
    if (ffmpeg->isRunning() && !nextFileName.isEmpty()) {
        ffmpeg->terminate();
        QThread::msleep(500);
        ffmpeg->init(nextFileName, listWidget, currentItem, this->ui->pBtnPlay, ui->horizontalSliderPlayProgress, ui->labelDuration, ui->labelNow);
        ffmpeg->start();
        playIcon.addFile("../resource/icon/pause.png");
        QString toolTip = "Pause";
        ui->pBtnPlay->setIcon(playIcon);
        ui->pBtnPlay->setToolTip(toolTip);
    }

    std::cout << nextFileName.toLocal8Bit().data() << std::endl;

}

void MainWindow::on_pBtnPlayMode_clicked() {

    std::cout << "isFinished = " << ffmpeg->isFinished() << std::endl;

    QIcon playModeIcon = ui->pBtnPlayMode->icon();
    QString toolTip;
    if (PLAY_MODE_ORDER == playMode) {
        playModeIcon.addFile("../resource/icon/shuffle.png");
        toolTip = "Shuffle";
        playMode = PLAY_MODE_SHUFFLE;
    } else if (PLAY_MODE_SHUFFLE == playMode) {
        playModeIcon.addFile("../resource/icon/single.png");
        toolTip = "Single";
        playMode = PLAY_MODE_SINGLE;
    } else if (PLAY_MODE_SINGLE == playMode) {
        playModeIcon.addFile("../resource/icon/repeat.png");
        toolTip = "Repeat";
        playMode = PLAY_MODE_REPEAT;
    } else if (PLAY_MODE_REPEAT == playMode) {
        playModeIcon.addFile("../resource/icon/order.png");
        toolTip = "Order";
        playMode = PLAY_MODE_ORDER;
    }
    ui->pBtnPlayMode->setIcon(playModeIcon);
    ui->pBtnPlayMode->setToolTip(toolTip);

    ffmpeg->setPlayMode(playMode);

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

void MainWindow::on_horizontalSliderPlayProgress_sliderMoved(int time) {

    QString now = QTime(0,0,0).addMSecs(time).toString(QString::fromStdString("hh:mm:ss"));
    ui->labelNow->setText(now);
    ffmpeg->sliderValueChange(time);

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

void MainWindow::playAudio(QListWidgetItem *item) {
    if (ffmpeg->isRunning()) ffmpeg->terminate();
    QThread::msleep(500);
    ffmpeg->init(item->text(), plDialog->getQListWidget(), item, ui->pBtnPlay, ui->horizontalSliderPlayProgress, ui->labelDuration, ui->labelNow);
    ffmpeg->start();
}

void MainWindow::pBtnPlayChange() {
    QIcon playIcon = ui->pBtnPlay->icon();
    playIcon.addFile("../resource/icon/pause.png");
    QString toolTip = "Pause";
    ui->pBtnPlay->setIcon(playIcon);
    ui->pBtnPlay->setToolTip(toolTip);
}
