#ifndef PLAYLISTDIALOG_H
#define PLAYLISTDIALOG_H

#include <QDialog>

namespace Ui {
class PlaylistDialog;
}

class PlaylistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlaylistDialog(QWidget *parent = nullptr);
    ~PlaylistDialog();

    QString getFirstOrSelectedFileName();

    QString getPrevFileName();

    QString getNextFileName();

signals:

    void playAudio(QString audioPath);

    void pBtnPlayChange();

private slots:
    void on_pBtnAdd_clicked();
    
    void on_pBtnRemove_clicked();

    void on_pBtnClear_clicked();

    void on_listWidget_doubleClicked();

private:
    Ui::PlaylistDialog *ui;

    QStringList *playlist;
};

#endif // PLAYLISTDIALOG_H
