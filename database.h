#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDebug>

//Класс осуществляет соединение с базой данных
//При отсутствии база создает пустую

class DataBase : public QObject
{
    Q_OBJECT

    QSqlDatabase    *db;
    QString         lastError;

public:
    explicit DataBase(QObject *parent = 0);
    bool connectToBase();
    QString lastErr() { return lastError; }

private:
    bool createBase(); // создаём пустую базу
    bool openBase();   // отерывает существующую

signals:

public slots:
};

#endif // DATABASE_H
