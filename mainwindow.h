#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlTableModel>
#include "db.h"

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
    void on_btnRefresh_clicked();

    void on_btnAdd_clicked();

    void on_btnRemove_clicked();

    void on_bntUp_clicked();

    void on_bntDown_clicked();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_btnSave_clicked();

    void on_pushButton_2_clicked();

    /*void on_actionAdd_Channel_triggered();

    void on_actionDown_channel_triggered();

    void on_actionUp_channel_triggered();*/

    void on_actionNew_triggered();

private:
    Ui::MainWindow *ui;
    void setupTableView(QSqlTableModel &model);
};
#endif // MAINWINDOW_H
