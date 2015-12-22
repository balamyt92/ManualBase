#ifndef MODELEDITDIALOG_H
#define MODELEDITDIALOG_H

#include <QDialog>
#include <QString>
#include <QSqlRelationalTableModel>
#include <QSqlError>
#include <QDebug>
#include <QInputDialog>
#include <QSqlQuery>
#include <QStringList>
#include <QRegExp>
#include <QMessageBox>

namespace Ui {
class ModelEditDialog;
}

class ModelEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelEditDialog(QWidget *parent = 0);
    ~ModelEditDialog();
    void setManualId(QString id);

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ModelEditDialog         *ui;
    QSqlRelationalTableModel    *model;
    QString                     mark;
    QString                     man;
};

#endif // MODELEDITDIALOG_H
