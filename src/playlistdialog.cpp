#include "../include/playlistdialog.h"
#include "../form/ui_playlistdialog.h"

PlaylistDialog::PlaylistDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaylistDialog)
{
    ui->setupUi(this);
}

PlaylistDialog::~PlaylistDialog()
{
    delete ui;
}
