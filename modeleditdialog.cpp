#include "modeleditdialog.h"
#include "ui_modeleditdialog.h"

ModelEditDialog::ModelEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelEditDialog)
{
    ui->setupUi(this);

    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    addModeFlag = false;
}

ModelEditDialog::~ModelEditDialog()
{
    delete ui;
}

void ModelEditDialog::setManualId(QString id)
{
    model = new QSqlRelationalTableModel(this);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("manualtomodel");
    model->setRelation(1, QSqlRelation("models", "ID", "Name"));
    ui->listView->setModel(model);
    ui->listView->setModelColumn(1);

    man = id;
    QSqlQuery query;
    query.prepare("SELECT ID_Model FROM manualtomodel WHERE ID_Man=" + id);
    query.exec();
    query.next();
    mark = query.value(0).toString();
    query.prepare("SELECT ID_Mark FROM models WHERE ID=" + mark);
    query.exec();
    query.next();
    mark = query.value(0).toString();

    model->setFilter("ID_Man=" + id);
    if (!model->select())
        qDebug() << model->lastError().text();
}

void ModelEditDialog::addMode(QStringList *id_list, QStringList *name_list)
{
    idList = id_list;
    nameList = name_list;
    listModel = new QStringListModel(this);
    listModel->setStringList(*nameList);

    addModeFlag = true;
}

void ModelEditDialog::on_pushButton_3_clicked()
{
    this->close();
}


void ModelEditDialog::on_pushButton_clicked()
{
    QSqlQuery query;
    QStringList id;
    QStringList name;

    //query.prepare("SELECT ID, Name FROM models WHERE ID_Mark=" + mark);
    query.prepare("SELECT ID, Name FROM models ORDER BY Name");
    query.exec();
    while (query.next()) {
        id << query.value(0).toString();
        name << query.value(1).toString();
    }
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Выберите модель"), tr(""), name,
                                         0, false, &ok);

    if (ok && !item.isEmpty()) {
        query.prepare("INSERT INTO manualtomodel (ID_Man, ID_Model) "
                      "VALUES(" + man + "," +
                      id.at(name.indexOf(QRegExp(item))) + ")");
        query.exec();
        model->select();
    }
}

void ModelEditDialog::on_pushButton_2_clicked()
{
    QModelIndex index = ui->listView->currentIndex();
    if(!index.isValid())
        return;
    QString name = model->index(index.row(), 1).data().toString();

    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText(tr("Удаление"));
    msg.setInformativeText(tr("Вы действительно хотите удалить ") + name + "?");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);

    int ret = msg.exec();
    if(ret == QMessageBox::Ok) {
        QSqlQuery query;
        query.prepare("SELECT ID FROM models WHERE Name='"+ name + "'");
        query.exec();
        query.next();
        QString modelID = query.value(0).toString();
        qDebug() << query.executedQuery();

        query.prepare("DELETE FROM manualtomodel WHERE ID_Man=" + man + " AND ID_Model=" +modelID);
        if(!query.exec()) {
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Ошибка"));
            msg.setInformativeText("Не могу удалить!\n"
                                   + query.lastError().text());
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
        qDebug() << man;
        qDebug() << name;
        qDebug() << modelID;
        qDebug() << query.executedQuery();


//        if(!model->removeRow(index.row())) {
//            qDebug() << model->lastError().text();
//        }
//        if(!model->submitAll())
//        {
//            msg.setIcon(QMessageBox::Critical);
//            msg.setText(tr("Ошибка"));
//            msg.setInformativeText("Не могу удалить!\n"
//                                   + model->lastError().text());
//            msg.setDefaultButton(QMessageBox::Ok);
//            msg.exec();
//        }
//        qDebug() << model->lastError().text();
        model->select();
    }
}
