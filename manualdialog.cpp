#include "manualdialog.h"
#include "ui_manualdialog.h"

ManualDialog::ManualDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManualDialog)
{
    ui->setupUi(this);
//    genValid = new QIntValidator(0, 1000, this);
//    ui->lineGenertion->setValidator(genValid);

    model = new QSqlTableModel(this);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("manual");
    if(!model->select()) {
        qDebug() << "Ошибка!! " + model->lastError().text();
    }

    editFoto = false;
    openToAdd = false;
    noFoto = false;
    idList.clear();
    modelList.clear();
}

ManualDialog::~ManualDialog()
{
    delete ui;
}

void ManualDialog::addManual(QString id_model, bool copy, int id_copy)
{
    // мапим
    currentModel = id_model;
    model->insertRow(model->rowCount());
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->addMapping(ui->lineName, 1);
    mapper->addMapping(ui->plainTextDisc, 2);
    mapper->addMapping(ui->lineAutor, 3);
    // путь до фала зададим позже
    //mapper->addMapping(ui->pathToFile, 4);
    mapper->addMapping(ui->lineURLDown, 5);
    mapper->addMapping(ui->lineURLPay, 6);
    mapper->addMapping(ui->lineYear, 7);
    mapper->addMapping(ui->lineGenertion, 8);
    mapper->addMapping(ui->lineHouse, 9);
    mapper->addMapping(ui->lineCountList, 10);
    mapper->addMapping(ui->lineSize, 11);
    mapper->addMapping(ui->lineType, 12);
    mapper->toLast();

    QSqlQuery query;
    if(currentModel != "0") {
        query.prepare("SELECT Name FROM models WHERE ID=" + currentModel);
        query.exec();
        query.next();
        ui->models->setText(query.value(0).toString());

        idList << currentModel;
        modelList << query.value(0).toString();
    }
    openToAdd = true;

    if(!copy) {
        // заполняем данными с прошлого ввода
        query.prepare("SELECT * FROM manual "
                      "WHERE ID = (SELECT MAX(ID) FROM manual)");
    } else {
        // иначе скопированными
        query.prepare("SELECT * FROM manual "
                      "WHERE ID = " + QString::number(id_copy));
    }
    if(!query.exec()) { qDebug() << "Err: не смог получить данные"; }
    while (query.next()) {
        ui->lineName->setText(query.value(1).toString());
        ui->plainTextDisc->setPlainText(query.value(2).toString());
        ui->lineAutor->setText(query.value(3).toString());
        ui->lineYear->setText(query.value(7).toString());
        ui->lineGenertion->setText(query.value(8).toString());
        ui->lineHouse->setText(query.value(9).toString());
        ui->lineCountList->setText(query.value(10).toString());
        ui->lineSize->setText(query.value(11).toString());
        ui->lineType->setText(query.value(12).toString());
        if(copy) {
            ui->lineURLDown->setText(query.value(5).toString());
            ui->lineURLPay->setText(query.value(6).toString());
            fileName = query.value(4).toString();
            ui->pathToFile->setText(fileName);
            editFoto = true;
            noFoto = false;
            QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView);
            scene->addPixmap(QPixmap(fileName).scaled(180, 250, Qt::KeepAspectRatio));
            ui->graphicsView->setScene(scene);
        }
    }


}

void ManualDialog::saveManual(QString id_mark, QString id_model)
{
    mapper->submit();
    model->submitAll();
    QSqlQuery query;
    query.prepare("SELECT ID FROM manual "
                  "WHERE ID = (SELECT MAX(ID) FROM manual)");
    query.exec();
    query.next();

    QString id_manual = query.value(0).toString();
            //model->index(model->rowCount() - 1, 0).data().toString();
    qDebug() << id_model;
    if(id_model == "0") {
        query.prepare("INSERT INTO manualtomodel (ID_Man, ID_Model, ID_Mark) VALUES("+
                      id_manual +","+ id_model +", " + id_mark + ")");
    } else {
        query.prepare("INSERT INTO manualtomodel (ID_Man, ID_Model, ID_Mark) VALUES("+
                      id_manual +","+ currentModel +","+ id_mark +")");
    }
    if(!query.exec()) {
        qDebug() << "Errr!!";
    }
}

void ManualDialog::editManual(QString id_manual, QString id_model)
{
    currentModel = id_model;
    model->setFilter("ID=" + id_manual);
    model->select();
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->addMapping(ui->lineName, 1);
    mapper->addMapping(ui->plainTextDisc, 2);
    mapper->addMapping(ui->lineAutor, 3);
    mapper->addMapping(ui->lineURLDown, 5);
    mapper->addMapping(ui->lineURLPay, 6);
    mapper->addMapping(ui->lineYear, 7);
    mapper->addMapping(ui->lineGenertion, 8);
    mapper->addMapping(ui->lineHouse, 9);
    mapper->addMapping(ui->lineCountList, 10);
    mapper->addMapping(ui->lineSize, 11);
    mapper->addMapping(ui->lineType, 12);
    mapper->toFirst();

    ui->pathToFile->setText(model->index(0,4).data().toString());
    QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView);
    scene->addPixmap(QPixmap(ui->pathToFile->text()).scaled(180, 250, Qt::KeepAspectRatio));
    ui->graphicsView->setScene(scene);
//    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);

    QString models;
    QSqlQuery query("SELECT models.Name FROM manualtomodel "
                    "LEFT JOIN models ON models.ID = manualtomodel.ID_Model "
                    "WHERE ID_Man=" + id_manual);
    while(query.next()) {
        models += query.value(0).toString() + " ";
    }

    ui->models->setText(models);

    if(ui->pathToFile->text() == "img/not_image.jpg") {
        ui->checkBox->setChecked(true);
        ui->selectFile->setDisabled(true);
        noFoto = true;
    }
}

void ManualDialog::updateManual()
{
    model->submitAll();
}

void ManualDialog::setPathToFile(QString path, QString path_prew)
{
    model->setData(model->index(model->rowCount() - 1, 4), path);
    model->setData(model->index(model->rowCount() - 1, 13), path_prew);
    model->submitAll();
}

void ManualDialog::on_selectFile_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Выберике обложку"), "",
                                                    tr("Изображения (*.png *.jpg *jpeg *.bmp)"));
    if(!fileName.isEmpty()) {
        editFoto = true;
        noFoto = false;
        ui->pathToFile->setText(fileName);
        QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView);
        scene->addPixmap(QPixmap(fileName).scaled(180, 250, Qt::KeepAspectRatio));
        ui->graphicsView->setScene(scene);
        //ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void ManualDialog::on_slectModel_clicked()
{
    if (openToAdd)
        return;

    ModelEditDialog *dialog = new ModelEditDialog(this);
    if(openToAdd) {
        dialog->addMode(&idList, &modelList);
    }
    else {
        dialog->setManualId(model->data(model->index(0,0)).toString());
    }

    dialog->exec();
    delete dialog;
}

void ManualDialog::on_lineURLPay_textChanged(const QString &arg1)
{
    // замена fish://user@84.22.143.158:22/mnt/ftp
    // на http://84.22.143.158/files
    QString tmp = arg1;
    tmp.replace("fish://user@84.22.143.158:22/mnt/ftp", "http://84.22.143.158/files");
    if(tmp != arg1) {
        ui->lineURLPay->setText(tmp);
    }
}

void ManualDialog::on_checkBox_toggled(bool checked)
{
    editFoto = true;
    ui->selectFile->setDisabled(checked);
    if(checked) {
        ui->pathToFile->setText("img/not_image.jpg");
        QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView);
        scene->addPixmap(QPixmap("img/not_image.jpg").scaled(180, 250, Qt::KeepAspectRatio));
        ui->graphicsView->setScene(scene);
        noFoto = true;
    }
}
