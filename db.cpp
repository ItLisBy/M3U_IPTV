#include "db.h"
#include <QChar>
#include <QtDebug>

QSqlDatabase DB::db = QSqlDatabase::addDatabase("QSQLITE");//Глобальный обьект базы
QSqlTableModel DB::model;
QString DB::startStr;
QSqlQuery* DB::query = new QSqlQuery(DB::db);

bool DB::initDB() {//Как открывем базу
    db.setDatabaseName("db.sqlite");//Название базы

    if (!db.open())//Проверка открытия
    {
        QMessageBox::warning(nullptr, "Error", "Connection failed: " + db.lastError().text(), QMessageBox::Ok);
        return false;
    }

    //query->exec("PRAGMA synchronous=OFF");
    resetTable();

    db.close();
    return true;
}

bool DB::fromDbToFile(QString filePath) {
    if(!db.open()) {
        QMessageBox::warning(nullptr, "Error", "Connection failed: " + db.lastError().text(), QMessageBox::Ok);
        return false;
    }

    QFile myFile (filePath);
    QTextStream stream; //Создаем поток


    myFile.open(QIODevice::WriteOnly);

    stream.setDevice(&myFile); // Подключение потока к файлу

    stream << "#EXTM3U" << startStr << "\r\n"; //Добавляем конец строки и переход

    if (!query->exec("SELECT * FROM Channels ORDER BY Num ASC;")) { //Проверочка
            qDebug() << "Unable to execute query - exiting";
    }

    //Reading of the data
    QSqlRecord rec = query->record();
    QString chName = nullptr;
    QString chGroup = nullptr;
    QString chUrl = nullptr;
    QString chRec = nullptr;

    while (query->next())
    {
        chName  = query->value(rec.indexOf("Name")).toString();
        chGroup = query->value(rec.indexOf("ChannelGroup")).toString();
        chUrl = query->value(rec.indexOf("URL")).toString();
        chRec = query->value(rec.indexOf("Rec")).toString();
        stream.setEncoding(QStringConverter::Utf8); //Ставим кодировку
        stream.setGenerateByteOrderMark(false);
        stream << "#EXTINF:0 tvg-rec=\"" <<  chRec << "\","
               << chName << "\n"
               << "#EXTGRP:" << chGroup << "\r\n"
               <<  chUrl << "\r\n"; //Выводим все разом в файл
    }
    myFile.close();
    db.close();
    return true;
}

//TODO:Review this shit
void DB::makeModel() {

    if (!db.isOpen()) {
        if (!db.open())
            return;
        model.setTable("Channels");//Имя тэйблы
        model.setEditStrategy(QSqlTableModel::OnManualSubmit);//Метод обновления таблицы после редактирования
        model.setSort(1, Qt::AscendingOrder);
        model.select(); // Делаем выборку значений из таблиц
        while (model.canFetchMore())
            model.fetchMore();
        db.close();
        emit modelChanged();
        return;
    }
    model.setTable("Channels");//Имя тэйблы
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);//Метод обновления таблицы после редактирования
    model.setSort(1, Qt::AscendingOrder);
    model.select(); // Делаем выборку значений из таблиц
    while (model.canFetchMore())
        model.fetchMore();
    emit modelChanged();
}

bool DB::fileToDB(QString filePath) {
    QFile myFile (filePath); //Устанавливаем что за файл
    myFile.open(QIODevice::ReadOnly); //ИЧСХ - открываем файл

    if(!db.open()) {
        QMessageBox::warning(nullptr, "Error", "Connection failed: " + db.lastError().text(), QMessageBox::Ok);
        return false;
    }

    resetTable();

    int strNumber = 0; //Line number;                          x
    int chanNumber = 0; //Channel number;                      y
    int strChanNumber = 0; //Number of line in channel's block z

    QString str = nullptr;
    QString chName = nullptr;
    QString chRec = NULL;
    QString chGroup = nullptr;
    QString chUrl = nullptr;
    QString	strDb = nullptr;

    bool flag = false;// Ну, надо (см. ниже)

    while (!myFile.atEnd()) {//Бежим по файлу
        flag = false; //Возвращаем в исходное положение
        str = myFile.readLine();
        str = str.simplified();//Выкидываем из линии все лишнее

        if (str.contains("#EXTM3U")) //Если есть первая строка
        {
            startStr = str.remove("#EXTM3U");
            //startStr = startStr.remove("\r\n");//Удаляем перевод каретки и сдвиг строки
            strNumber++;
        }

        else if (str.contains("#EXTINF")) //Узнаем имя
        {
            chRec = str[str.indexOf("\"")+1];
            chName = str.remove(0, str.indexOf(',') + 1);
            //chName = chName.remove("\r\n");
            chName = chName.remove("'");
            strNumber++;
            chanNumber++;
            strChanNumber++;
        }

        else if (str.contains("#EXTGRP:"))
        {
            chGroup = str.remove("#EXTGRP:");
            //chGroup = chGroup.remove("\r\n");
            strNumber++;
            strChanNumber++;
        }

        else if (str.contains("http"))
        {
            chUrl = str;
            //chUrl = chUrl.remove("\r\n");
            strNumber++;
            strChanNumber = 0;
        }
        //TODO:Review this shit
        else if ((!str.startsWith("0x0A") || !str.startsWith("\r") || !str.startsWith("\n")) && strNumber != 0)
        {
            QMessageBox *errorMessage = new QMessageBox;
            QString *errText = new QString ("Я не умею реагировать на такую информацию)\nString N" + QString::number(strNumber + 1));
            errorMessage->setText(*errText);
            errorMessage->exec();
            delete errorMessage;
            delete errText;
            QString strF = "INSERT INTO  Channels (Num, Name, ChannelGroup, URL, Rec) "
                           "VALUES(%1, '%2', '%3', '%4', '%5');";//Загружаем ересь в базу

            strDb = strF.arg(strNumber+1)
                        .arg("Ересь")
                        .arg("Ересь")
                        .arg("Ересь")
                        .arg(0);

            if (!query->exec(strDb)){ //Проверочка
                qDebug() << "0Unable to do insert opeation" + QString::number(strNumber);
            }

            strNumber++;
            flag = true;
        }
        // Вот сюда смотри-----------------------------|
        //                                             V
        if (strChanNumber == 0 && chanNumber != 0 && flag != true) //Иначе считает что 1 строка - канал и загружает его
        {
            QString strF = "INSERT INTO  Channels (Num, Name, ChannelGroup, URL, Rec) "
                           "VALUES(%1, '%2', '%3', '%4', '%5');";//Загружаем дату о канале в базу

            strDb = strF.arg(chanNumber)
                        .arg(chName.remove("'"))
                        .arg(chGroup)
                        .arg(chUrl)
                        .arg(chRec);

            if (!query->exec(strDb)) //Проверочка
            {
                qDebug() << "Unable to do insert opeation1";
            }
        }

    }
    myFile.close();
    makeModel();
    db.close();
    return true;
}

bool DB::resetTable() {
    query->exec("DROP TABLE Channels");
    query->exec("CREATE TABLE IF NOT EXISTS 'Channels'(\
                                           'ChannelID'	INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,\
<<<<<<< HEAD
                                           'Num' INTEGER,\
                                           'Name' TEXT NOT NULL,\
                                           'ChannelGroup' TEXT ,\
                                           'Rec' TEXT DEFAULT 0,\
                                           'URL' TEXT \
=======
                                           'Num' INTEGER UNIQUE,\
                                           'Name' TEXT,\
                                           'ChannelGroup' TEXT,\
                                           'URL' TEXT,\
                                           'Rec' TEXT DEFAULT 0\
>>>>>>> refs/remotes/origin/master
                                           )");
    return true;
}

<<<<<<< HEAD
void DB::save()
{
    if(!db.open()) {
        QMessageBox::warning(nullptr, "Error", "Connection failed: " + db.lastError().text(), QMessageBox::Ok);
    }
    model.submitAll();
    db.close();
}

void DB::discard()
{
    if(!db.open()) {
        QMessageBox::warning(nullptr, "Error", "Connection failed: " + db.lastError().text(), QMessageBox::Ok);
    }
    model.revertAll();
    db.close();
}

void DB::insertEmptyRecord(int pos)
{
    if(!db.open()) {
        QMessageBox::warning(nullptr, "Error", "Connection failed: " + db.lastError().text(), QMessageBox::Ok);
    }
    query->prepare("UPDATE Channels SET Num = Num + 1 WHERE Num > ?;");
    query->addBindValue(pos+1);
    query->exec();
    query->prepare("INSERT INTO Channels (Num, Name, ChannelGroup, URL, Rec)"
                   "VALUES(?, 'new', '', '', '0');");
    query->addBindValue(pos+2);
    query->exec();
    db.close();
}

void DB::rmRecord(int pos)
{
    if(!db.open()) {
        QMessageBox::warning(nullptr, "Error", "Connection failed: " + db.lastError().text(), QMessageBox::Ok);
    }
    query->prepare("DELETE FROM Channels WHERE Num = ?;");
    query->addBindValue(pos+1);
    query->exec();
    query->prepare("UPDATE Channels SET Num = Num - 1 WHERE Num > ?;");
    query->addBindValue(pos+1);
    query->exec();
    db.close();
}

void DB::mvDownRecord(int from)
{
    if(!db.open()) {
        QMessageBox::warning(nullptr, "Error", "Connection failed: " + db.lastError().text(), QMessageBox::Ok);
    }
    query->prepare("UPDATE Channels SET Num = -1 WHERE Num = ?;");
    query->addBindValue(from+1);
    query->exec();

    query->prepare("UPDATE Channels SET Num = ? WHERE Num = ?;");
    query->addBindValue(from+1);
    query->addBindValue(from+2);
    query->exec();

    query->prepare("UPDATE Channels SET Num = ? WHERE Num = -1;");
    query->addBindValue(from+2);
    query->exec();
    db.close();
}

void DB::mvUpRecord(int from)
{
    if(!db.open()) {
        QMessageBox::warning(nullptr, "Error", "Connection failed: " + db.lastError().text(), QMessageBox::Ok);
    }
    query->prepare("UPDATE Channels SET Num = -1 WHERE Num = ?;");
    query->addBindValue(from+1);
    query->exec();

    query->prepare("UPDATE Channels SET Num = ? WHERE Num = ?;");
    query->addBindValue(from+1);
    query->addBindValue(from);
    query->exec();

    query->prepare("UPDATE Channels SET Num = ? WHERE Num = -1;");
    query->addBindValue(from);
    query->exec();
    db.close();
}

void DB::newPlaylist()
{
    insertEmptyRecord(-1);

=======
void DB::addEmptyLine(int pos) {
    QString strF = "INSERT INTO  Channels (Num, Name, ChannelGroup, URL, Rec) "
                   "VALUES(%1, '%2', '%3', '%4', '%5');";//Загружаем ересь в базу

    strF = strF.arg(pos)
                .arg("")
                .arg("")
                .arg("")
                .arg("");
>>>>>>> refs/remotes/origin/master
}
