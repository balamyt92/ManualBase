#ifndef HTMLMAKER_H
#define HTMLMAKER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class HTMLMaker : public QObject
{
    Q_OBJECT

    QString path;
    QString id_mark;
    QString man_template;
    QString menu_elem;
    QString menu_template;


public:
    explicit HTMLMaker(QObject *parent = 0);
    void setPath(QString _path);
    bool run(QString _id_mark);

private:
    QString makeMan(QList<QString> data, QString template_);
    QString makePage(QString id_model);
    QString makeMenu();
signals:

public slots:
};

#endif // HTMLMAKER_H
