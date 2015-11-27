#ifndef MANUALDIALOG_H
#define MANUALDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QString>
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QSqlError>
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
    void setCurrentModel(QString model);

private:
    Ui::ManualDialog  *ui;
    QValidator        *genValid;
    QDataWidgetMapper *mapper;
    QSqlTableModel    *model;
};

#endif // MANUALDIALOG_H
