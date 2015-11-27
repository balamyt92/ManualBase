#include "manualdialog.h"
#include "ui_manualdialog.h"

ManualDialog::ManualDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManualDialog)
{
    ui->setupUi(this);
    genValid = new QIntValidator(0, 1000, this);
    ui->lineGenertion->setValidator(genValid);

    model = new QSqlTableModel(this);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("manual");
    if(!model->select()) {
        qDebug() << "Ошибка!! " + model->lastError().text();
    }
}

ManualDialog::~ManualDialog()
{
    delete ui;
}

void ManualDialog::setCurrentModel(QString model)
{

}
