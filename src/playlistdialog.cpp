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
    QString filter = "Suppoerted Audio Types(*.aac *.ac3 *.ape *.flac *.mp3 *.ogg *.wav *.wma)";
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

void PlaylistDialog::on_listWidget_doubleClicked() {

    std::cout << ui->listWidget->currentItem()->text().toStdString() << std::endl;

    QString audioPath = ui->listWidget->currentItem()->text();

    emit playAudio(audioPath);

    emit pBtnPlayChange();

}

QString PlaylistDialog::getFirstOrSelectedFileName() {
    QString currentOrFirstFile;
    QListWidget *listWidget = ui->listWidget;
    if (listWidget) {
        QListWidgetItem *currentItem = ui->listWidget->currentItem();
        if (currentItem) {
            currentOrFirstFile = currentItem->text();
        } else {
            currentItem = listWidget->item(0);
            currentItem->setSelected(true);
            listWidget->setCurrentItem(currentItem);
            currentOrFirstFile = currentItem->text();
        }
    }
    return currentOrFirstFile;
}

QString PlaylistDialog::getPrevFileName() {
    QString prevFile;
    QListWidget *listWidget = ui->listWidget;
    if (listWidget && listWidget->count() > 1) {
        QListWidgetItem *currentItem = ui->listWidget->currentItem();
        if (currentItem) {
            int curRow = listWidget->row(currentItem);
            if (curRow > 0) {
                currentItem = listWidget->item(curRow - 1);
                currentItem->setSelected(true);
                listWidget->setCurrentItem(currentItem);
                prevFile = currentItem->text();
            }
        }
    }
    return prevFile;
}

QString PlaylistDialog::getNextFileName() {
    QString nextFile;
    QListWidget *listWidget = ui->listWidget;
    if (listWidget) {
        int count = listWidget->count();
        if (count > 1) {
            QListWidgetItem *currentItem = ui->listWidget->currentItem();
            if (currentItem) {
                int curRow = listWidget->row(currentItem);
                if (curRow < count - 1) {
                    currentItem = listWidget->item(curRow + 1);
                    currentItem->setSelected(true);
                    listWidget->setCurrentItem(currentItem);
                    nextFile = currentItem->text();
                }
            }
        }
    }
    return nextFile;
}
