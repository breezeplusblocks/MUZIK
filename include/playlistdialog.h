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

private slots:
    void on_pBtnAdd_clicked();
    
    void on_pBtnRemove_clicked();

    void on_pBtnClear_clicked();

private:
    Ui::PlaylistDialog *ui;

    QStringList *playlist;


};

#endif // PLAYLISTDIALOG_H
