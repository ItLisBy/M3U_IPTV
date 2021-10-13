#ifndef DB_H
#define DB_H

#include <QtSql>
#include <QMessageBox>


class DB {
    Q_OBJECT
public:

    static QSqlDatabase db;//Глобальный обьект базы

    static QSqlTableModel model;

    static QSqlQuery *query;

    static bool initDB();//Открываем базу

    static bool fromDbToFile(QString filePath);

    static void makeModel();

    static bool fileToDB(QString filePath);

    static QString startStr;

    static bool resetTable();

    static void addEmptyLine(int pos);

signals:

    static void modelChanged();
};

#endif // DB_H
