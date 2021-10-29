#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

    void setLabelNowValue(int time);

    void volumeChange(int volume);

private:
    Ui::MainWindow *ui;

    bool playStatus;

    int playMode;

    int musicDuration;

    int volume;

    bool muteStatus;
};
#endif // MAINWINDOW_H
