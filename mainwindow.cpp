/*
 * TODO:Restrict Num from setting under 1 by user
 * TODO:Unsaved changes notification/Unsaved changes indicator
 * TODO:SPEEEEEEEEEEEEEEEEEEEEEEEEEEEED IT UP!
 * TODO:DB's errors handling
 * TODO:try/catches
 * TODO:Drag 'n drop
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnRefresh_clicked() {
    DB::makeModel();
    setupTableView(DB::model);
}

//TODO:ID and Num should be auto-setting on row adding
void MainWindow::on_btnAdd_clicked()
{
    if (ui->tableView->selectionModel() == nullptr)
        return;
    auto rows = ui->tableView->selectionModel()->selectedRows();
    if (rows.length() == 0)
        return;
    auto index = rows.at(rows.length()-1);
    DB::insertEmptyRecord(index.row());
    on_btnRefresh_clicked();
}


void MainWindow::on_btnRemove_clicked()
{
    if (ui->tableView->selectionModel() == nullptr)
        return;
    auto rows = ui->tableView->selectionModel()->selectedRows();
    if (rows.length() == 0)
        return;
    auto index = rows.at(rows.length()-1);
    DB::rmRecord(index.row());
    on_btnRefresh_clicked();
}


void MainWindow::on_bntUp_clicked()
{
    if (ui->tableView->selectionModel() == nullptr)
        return;
    auto rows = ui->tableView->selectionModel()->selectedRows();
    if (rows.length() == 0)
        return;
    auto index = rows.at(rows.length()-1);
    if (index.row() != 0){
        DB::mvUpRecord(index.row());
        on_btnRefresh_clicked();
    }
}


void MainWindow::on_bntDown_clicked()
{
    if (ui->tableView->selectionModel() == nullptr)
        return;
    auto rows = ui->tableView->selectionModel()->selectedRows();
    if (rows.length() == 0)
        return;
    auto index = rows.at(rows.length()-1);
    if (index.row()+1 != DB::model.rowCount()){
        DB::mvDownRecord(index.row());
        on_btnRefresh_clicked();
    }
}

void MainWindow::setupTableView(QSqlTableModel &model) {
    ui->tableView->setModel(&model);     // Устанавливаем модель на TableView

    ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->tableView->setColumnHidden(1, true);
    // Устанавливаем размер колонок по содержимому
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    this->resize(this->width()+1, this->height());
    this->resize(this->width()-1, this->height());
}


void MainWindow::on_actionOpen_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open File",
                                                    "",
                                                    "m3u (*.m3u*)");
    DB::fileToDB(filePath);
    on_btnRefresh_clicked();
}


void MainWindow::on_actionSave_triggered()
{
    QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    "untitled.m3u",
                                                    tr("m3u (*.m3u*)"));
    DB::fromDbToFile(savePath);
}


void MainWindow::on_btnSave_clicked()
{
    DB::save();
}


void MainWindow::on_pushButton_2_clicked()
{
    DB::discard();
}


void MainWindow::on_actionNew_triggered()
{
    DB::newPlaylist();
    on_btnRefresh_clicked();
}

