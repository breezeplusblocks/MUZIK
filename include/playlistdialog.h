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

private:
    Ui::PlaylistDialog *ui;
};

#endif // PLAYLISTDIALOG_H
