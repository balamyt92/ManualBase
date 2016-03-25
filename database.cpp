#include "database.h"

DataBase::DataBase(QObject *parent) : QObject(parent)
{

}

bool DataBase::connectToBase()
{
    QFile fileOfBase("base.db3");
    if(!fileOfBase.exists()) {
        qDebug() << "Не найден фал базы, он будет создан";
        if(!openBase()) {
            return false;
        }
        if(!createBase()) {
            return false;
        }
    }
    else {
        if(!openBase()) {
            return false;
        }
    }

    // Проверяем если папка для фоток
    QDir foto("img");
    if(!foto.exists()) {
        qDebug() << "Нет папки под фото!";
        foto.cdUp();
        foto.mkdir("img");
        foto.cd("img");
    }

    return true;
}

bool DataBase::createBase()
{
    QFile sheme(":/full_sheme.txt");
    if(!sheme.open(QIODevice::ReadOnly | QIODevice::Text)) {
        lastError = sheme.errorString();
        return false;
    }
    QTextStream in(&sheme);
    QStringList queryList = in.readAll().split(";");
    if(queryList.last() == "\n") {
        queryList.removeLast();
    }

    int i = 0;
    QSqlQuery query;
    while(i < queryList.size()) {
        query.prepare(queryList.at(i));
        if(!query.exec()) {
            lastError = "Ошибка содания базы: " + query.lastError().text()
                                + "\nЗапрос:\n" + query.lastQuery();
            qDebug() << lastError;
            return false;
        }
        i++;
    }
    return true;
}

bool DataBase::openBase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("base.db3");
    if(!db.open()) {
        lastError = "Не могу подключиться к базе! " + db.lastError().text();
        qDebug() << lastError;
        return false;
    }

    // включаем внешние ключи в базе
    QSqlQuery query("PRAGMA foreign_keys = ON");
    return true;
}
