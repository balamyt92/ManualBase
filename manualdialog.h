#ifndef MANUALDIALOG_H
#define MANUALDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QString>
#include <QStringList>
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QFileDialog>
#include <QPixmap>
#include <QImage>
#include <QGraphicsScene>

#include "modeleditdialog.h"

namespace Ui {
class ManualDialog;
}

class ManualDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManualDialog(QWidget *parent = 0);
    ~ManualDialog();
    void addManual(QString id_model, bool copy, int id_copy);
    void saveManual(QString id_mark, QString id_model);
    void editManual(QString id_manual, QString id_model);
    void updateManual();
    QString getFoto() { return fileName; }
    bool isEditFoto() { return editFoto; }
    bool isNoFoto() { return noFoto; }

    QString getIDLastAddManual() {
        QSqlQuery last;
        last.prepare("SELECT MAX(ID) FROM manual");
        last.exec();
        last.next();
        return last.value(0).toString();
        //model->index(model->rowCount() - 1, 0).data().toString();
    }

    void setPathToFile(QString path, QString path_prew);

private slots:
    void on_selectFile_clicked();

    void on_slectModel_clicked();

    void on_lineURLPay_textChanged(const QString &arg1);

    void on_checkBox_toggled(bool checked);

private:
    Ui::ManualDialog  *ui;
    QValidator        *genValid;
    QDataWidgetMapper *mapper;
    QSqlTableModel    *model;
    QString            currentModel;
    QString            fileName;
    bool               editFoto;
    bool               openToAdd;
    bool               noFoto;
    QStringList        modelList;
    QStringList        idList;
};

#endif // MANUALDIALOG_H
