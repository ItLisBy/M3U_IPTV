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

private:
    Ui::MainWindow *ui;
    void setupTableView(QSqlTableModel &model);
};
#endif // MAINWINDOW_H
