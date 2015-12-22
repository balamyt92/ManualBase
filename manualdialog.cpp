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

    editFoto = false;
}

ManualDialog::~ManualDialog()
{
    delete ui;
}

void ManualDialog::addManual(QString id_model)
{
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

    QSqlQuery query("SELECT Name FROM models WHERE ID=" + currentModel);
    query.next();
    ui->models->setText(query.value(0).toString());
}

void ManualDialog::saveManual()
{
    model->submitAll();
    QString id_manual = model->index(model->rowCount() - 1, 0).data().toString();
    QSqlQuery query;
    query.prepare("INSERT INTO manualtomodel (ID_Man, ID_Model) VALUES("+ id_manual +","+ currentModel +")");
    query.exec();
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
}

void ManualDialog::updateManual()
{
    model->submitAll();
}

void ManualDialog::setPathToFile(QString path)
{
    model->setData(model->index(model->rowCount() - 1, 4), path);
    model->submitAll();
}

void ManualDialog::on_selectFile_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Выберике обложку"), "",
                                                    tr("Изображения (*.png *.jpg *jpeg *.bmp)"));
    if(!fileName.isEmpty()) {
        editFoto = true;
        ui->pathToFile->setText(fileName);
        QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView);
        scene->addPixmap(QPixmap(fileName).scaled(180, 250, Qt::KeepAspectRatio));
        ui->graphicsView->setScene(scene);
        //ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void ManualDialog::on_slectModel_clicked()
{
    ModelEditDialog *dialog = new ModelEditDialog(this);
    dialog->setManualId(model->data(model->index(0,0)).toString());
    dialog->exec();
    delete dialog;
}
