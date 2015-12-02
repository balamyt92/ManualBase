#ifndef MANUALDIALOG_H
#define MANUALDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QString>
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

namespace Ui {
class ManualDialog;
}

class ManualDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManualDialog(QWidget *parent = 0);
    ~ManualDialog();
    void addManual(QString id_model);
    void saveManual();
    void editManual(QString id_manual, QString id_model);
    void updateManual();

private:
    Ui::ManualDialog  *ui;
    QValidator        *genValid;
    QDataWidgetMapper *mapper;
    QSqlTableModel    *model;
    QString           currentModel;
};

#endif // MANUALDIALOG_H
