#include "../include/playlistdialog.h"
#include "../form/ui_playlistdialog.h"

#include <iostream>
#include <QDir>
#include <QFileDialog>


PlaylistDialog::PlaylistDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaylistDialog)
{
    ui->setupUi(this);
    playlist = new QStringList;
}

PlaylistDialog::~PlaylistDialog()
{
    delete ui;
}

void PlaylistDialog::on_pBtnAdd_clicked() {

    QString caption = "Choose Audio File(s)";
    QString dir = QDir::homePath();
    QString filter = "Suppoerted Audio Types(*.wav *.mp3 *.wma)";
    QStringList files = QFileDialog::getOpenFileNames(this, caption, dir, filter);

    if (files.isEmpty())
        return;

    ui->listWidget->addItems(files);

}

void PlaylistDialog::on_pBtnRemove_clicked() {

    ui->listWidget->takeItem(ui->listWidget->currentRow());

}

void PlaylistDialog::on_pBtnClear_clicked() {

    ui->listWidget->clear();

}
