#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    ui->setupUi(this);
    DB a;
    QObject::connect((QObject*)&a, SIGNAL(DB::modelChanged()),
                     (QObject*)ui, SLOT(MainWindow::on_btnRefresh_clicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnRefresh_clicked() {
    //DB::makeModel();
    setupTableView(DB::model);
}


void MainWindow::on_btnAdd_clicked() {
    DB::model.insertRecord(DB::model.rowCount()+1, QSqlRecord());
}


void MainWindow::on_btnRemove_clicked()
{

}


void MainWindow::on_bntUp_clicked()
{

}


void MainWindow::on_bntDown_clicked()
{

}

void MainWindow::setupTableView(QSqlTableModel &model) {
    ui->tableView->setModel(&model);     // Устанавливаем модель на TableView

    ui->tableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    // Устанавливаем размер колонок по содержимому
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}


void MainWindow::on_actionOpen_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open File",
                                                    "",
                                                    "m3u (*.m3u*)");
    DB::fileToDB(filePath);
}

