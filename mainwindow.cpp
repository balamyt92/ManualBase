#include "mainwindow.h"
#include "database.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    rootURL = "http://www.autoinfo24.ru/rukovodstva-po-remontu/";
    copyMan = false;
    currentModel = "0";

    DataBase *db = new DataBase(this);
    if(!db->connectToBase()) {
        QMessageBox::critical(this, tr("Ошибка!"), db->lastErr());
        this->setEnabled(false);
    }
    else {
        // Разделы
        sectionModel = new QSqlTableModel(this);
        sectionModel->setTable("sections");
        sectionModel->setSort(1, Qt::AscendingOrder);
        if(!sectionModel->select()) {
            QString err = tr("Не могу считать разделы! <br>") + sectionModel->lastError().text();
            QMessageBox::critical(this, tr("Ошибка!"), err);
        }
        //sectionModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        //ui->SectionsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->SectionsListView->setModel(sectionModel);
        ui->SectionsListView->setModelColumn(1);

        // Марки
        markModel = new QSqlTableModel(this);
        markModel->setTable("marks");
        markModel->setSort(1, Qt::AscendingOrder);
        //markModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        //ui->MarksListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->MarksListView->setModel(markModel);
        ui->MarksListView->setModelColumn(1);
        ui->groupBox_2->setEnabled(false);

        // Модели
        modelModel = new QSqlTableModel(this);
        modelModel->setTable("models");
        modelModel->setSort(1, Qt::AscendingOrder);
        //modelModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        //ui->ModelsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->ModelsListView->setModel(modelModel);
        ui->ModelsListView->setModelColumn(1);
        ui->groupBox_3->setEnabled(false);

        // Мануалы
        manModel = new QSqlRelationalTableModel(this);
        manModel->setJoinMode(QSqlRelationalTableModel::LeftJoin);
        manModel->setTable("manualtomodel");
        manModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        manModel->setRelation(0, QSqlRelation("manual", "ID", "Name, ID, Generetion"));
        // TODO добавить сортировку мануалов по поколению
        // пока не понятно как setSort Не работает
        ui->ManualsListView->setModel(manModel);
        ui->ManualsListView->setModelColumn(0);
        ui->ManualsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->groupBox_4->setEnabled(false);
    }

    QSqlQuery query;
    query.prepare("INSERT INTO models (ID, Name, ID_Mark) VALUES(0, 'ALL', NULL)");
    query.exec();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_SectionsListView_clicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    ui->groupBox_3->setEnabled(false);
    ui->groupBox_4->setEnabled(false);

    currenSection = sectionModel->index(index.row(), 0).data().toString();
    markModel->setFilter("ID_Sections=" + currenSection);
    if(!markModel->select()) {
        QString err = tr("Не могу считать список марок! <br>") + markModel->lastError().text();
        QMessageBox::critical(this, tr("Ошибка!"), err);
    }
    else {
        ui->groupBox_2->setEnabled(true);
    }

}

void MainWindow::on_MarksListView_clicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    ui->groupBox_4->setEnabled(false);

    currentMark = markModel->index(index.row(), 0).data().toString();
    modelModel->setFilter("ID_Mark=" + currentMark);
    if(!modelModel->select()) {
        QString err = tr("Не могу считать список моделей! <br>") + modelModel->lastError().text();
        QMessageBox::critical(this, tr("Ошибка!"), err);
    }
    else {
        ui->groupBox_3->setEnabled(true);
    }
    manModel->setFilter("ID_Mark=" + currentMark + " AND ID_Model=0");
    if(!manModel->select()) {
        QString err = tr("Не могу считать список мануалов! <br>") + manModel->lastError().text();
        QMessageBox::critical(this, tr("Ошибка!"), err);
    }
    else {
        ui->groupBox_4->setEnabled(true);
    }
    currentModel = "0";
}

void MainWindow::on_markDel_clicked()
{
    QModelIndex index = ui->MarksListView->currentIndex();
    if(!index.isValid())
        return;
    QString name = markModel->index(index.row(), 1).data().toString();

    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText(tr("Удаление"));
    msg.setInformativeText(tr("Вы действительно хотите удалить ") + name + "?");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);
    int ret = msg.exec();
    if(ret == QMessageBox::Ok) {
        markModel->removeRow(index.row());
        if(!markModel->submitAll())
        {
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Ошибка"));
            msg.setInformativeText("Не могу удалить!\nВозможно есть вложенные модели\n"
                                   + markModel->lastError().text());
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
        markModel->select();
    }
}

void MainWindow::on_markAdd_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Новая марка"),
                                         tr("Наименвание марки:"), QLineEdit::Normal,
                                         "марка", &ok);
    if (ok && !text.isEmpty()) {
        int row = markModel->rowCount();
        markModel->insertRow(row);
        markModel->setData(markModel->index(row, 1), text);
        markModel->setData(markModel->index(row, 2), currenSection);
        markModel->submitAll();
    }
}


void MainWindow::on_sectionAdd_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Новый разде"),
                                         tr("Наименвание раздела:"), QLineEdit::Normal,
                                         "раздел", &ok);
    if (ok && !text.isEmpty()) {
        int row = sectionModel->rowCount();
        sectionModel->insertRow(row);
        sectionModel->setData(sectionModel->index(row, 1), text);
        sectionModel->submitAll();
    }
}

void MainWindow::on_sectionDel_clicked()
{
    QModelIndex index = ui->SectionsListView->currentIndex();
    if(!index.isValid())
        return;
    QString name = sectionModel->index(index.row(), 1).data().toString();

    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText(tr("Удаление"));
    msg.setInformativeText(tr("Вы действительно хотите удалить ") + name + "?");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);
    int ret = msg.exec();
    if(ret == QMessageBox::Ok) {
        sectionModel->removeRow(index.row());
        if(!sectionModel->submitAll())
        {
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Ошибка"));
            msg.setInformativeText("Не могу удалить!\nВозможно есть вложенные марки \n"
                                   + markModel->lastError().text());
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
        sectionModel->select();
    }
}

void MainWindow::on_ModelAdd_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Новая модель"),
                                         tr("Наименвание модели:"), QLineEdit::Normal,
                                         "модель", &ok);
    if (ok && !text.isEmpty()) {
        int row = modelModel->rowCount();
        modelModel->insertRow(row);
        modelModel->setData(modelModel->index(row, 1), text);
        modelModel->setData(modelModel->index(row, 2), currentMark);
        modelModel->submitAll();
    }
}

void MainWindow::on_modelDel_clicked()
{
    QModelIndex index = ui->ModelsListView->currentIndex();
    if(!index.isValid())
        return;
    QString name = modelModel->index(index.row(), 1).data().toString();

    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText(tr("Удаление"));
    msg.setInformativeText(tr("Вы действительно хотите удалить ") + name + "?");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);
    int ret = msg.exec();
    if(ret == QMessageBox::Ok) {
        modelModel->removeRow(index.row());
        if(!modelModel->submitAll())
        {
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Ошибка"));
            msg.setInformativeText("Не могу удалить!\nВозможно есть вложенные мануалы \n"
                                   + modelModel->lastError().text());
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
        modelModel->select();
    }
}

void MainWindow::on_ModelsListView_clicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    ui->groupBox_4->setEnabled(true);

    currentModel = modelModel->index(index.row(), 0).data().toString();
    manModel->setFilter("ID_Model=" + currentModel);
    if(!manModel->select()) {
        QString err = tr("Не могу считать список мануалов! <br>") + manModel->lastError().text();
        QMessageBox::critical(this, tr("Ошибка!"), err);
    }
}

void MainWindow::on_ManualsListView_doubleClicked(const QModelIndex &index)
{
    QString id_man = manModel->index(index.row(), 1).data().toString();
    ManualDialog *md = new ManualDialog(this);
    md->editManual(id_man, currentModel);
    if(md->exec() == QDialog::Accepted)
    {
        md->updateManual();
        manModel->select();

        if(md->isEditFoto()) {
            if(md->isNoFoto()) {
                md->setPathToFile("img/not_image.jpg", "img/not_image.jpg");
            } else {
                QFile::remove(QDir::currentPath() + "/img/" + id_man + ".jpg");
                QFile::remove(QDir::currentPath() + "/img/prew_" + id_man + ".jpg");
                QFile::copy(md->getFoto(), QDir::currentPath() + "/img/" +
                            id_man + ".jpg");

                QImage img(md->getFoto());
                img = img.scaled(QSize(120, 170), Qt::KeepAspectRatio, Qt::SmoothTransformation);

                if(!img.save(QDir::currentPath() + "/img/prew_" +
                         id_man + ".jpg"))
                    qDebug() << "Fail!!!!!";
                md->setPathToFile("img/" + id_man + ".jpg", "/img/prew_" + id_man + ".jpg");
            }
        }
    }
    delete md;
}

void MainWindow::on_manAdd_clicked()
{
    ManualDialog *md = new ManualDialog(this);
    md->addManual(currentModel, copyMan, copyIdMan);
    if(md->exec() == QDialog::Accepted)
    {
        copyMan = false;
        ui->statusBar->showMessage("");
        md->saveManual(currentMark, currentModel);
        manModel->select();
        if(md->isNoFoto()) {
            md->setPathToFile("img/not_image.jpg", "img/not_image.jpg");
            QSqlQuery update;
            update.prepare("UPDATE manual SET IMG=\'img/not_image.jpg\', IMG_prew=\'img/not_image.jpg\' WHERE ID=(SELECT MAX(ID) FROM manual)");
            if(!update.exec()) qDebug() << "Fail! save foto " << update.lastError().text();
        } else {
            QFile::copy(md->getFoto(), QDir::currentPath() + "/img/" +
                        md->getIDLastAddManual() + ".jpg");

            QImage img(md->getFoto());
            img = img.scaled(QSize(120, 170), Qt::KeepAspectRatio, Qt::SmoothTransformation);

            if(!img.save(QDir::currentPath() + "/img/prew_" +
                     md->getIDLastAddManual() + ".jpg"))
                qDebug() << "Fail!!!!!";

            QSqlQuery update;
            update.prepare("UPDATE manual SET IMG=\'img/" + md->getIDLastAddManual() + ".jpg\'," +
                                             "IMG_prew=\'img/prew_" + md->getIDLastAddManual() + ".jpg\' " +
                                             "WHERE ID=(SELECT MAX(ID) FROM manual)");
            if(!update.exec()) qDebug() << "Fail! save foto " << update.lastError().text();
            md->setPathToFile("img/" + md->getIDLastAddManual() + ".jpg",
                              "img/prew_" + md->getIDLastAddManual() + ".jpg");
        }
    }
    delete md;
}

void MainWindow::on_manDel_clicked()
{
    QModelIndex index = ui->ManualsListView->currentIndex();
    if(!index.isValid())
        return;

    QString id_man = manModel->index(index.row(), 1).data().toString();

    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText(tr("Удаление"));
    msg.setInformativeText(tr("Вы действительно хотите удалить мануал ") + index.data().toString() + "?");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);
    int ret = msg.exec();
    if(ret == QMessageBox::Ok) {
        QSqlQuery query;
        query.prepare("DELETE FROM manualtomodel WHERE ID_Man=" + id_man + " AND ID_Model=" + currentModel);
        if(!query.exec()) {
            goto ERR;
        }

        QFile::remove(QDir::currentPath() + "/img/" + id_man + ".jpg");
        QFile::remove(QDir::currentPath() + "/img/prew_" + id_man + ".jpg");

        query.prepare("DELETE FROM manual WHERE ID=" + id_man);
        if(!query.exec())
        {
            ERR:
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Ошибка"));
            msg.setInformativeText("Не могу удалить!\n"
                                   + query.lastError().text());
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
        manModel->select();

        QFile::remove(QDir::currentPath() + "/img/" + id_man + ".jpg");
    }
}


#include "htmlmaker.h"
void MainWindow::on_pushButton_clicked()
{
    QModelIndex index = ui->MarksListView->currentIndex();
    if(!index.isValid())
        return;

    QString id_mark = markModel->index(index.row(), 0).data().toString();
    qDebug() << id_mark;
    QString path = QDir::currentPath() + "/out";
    QDir dir;
    dir.mkdir(path);
    dir.mkdir(path + "/img");

    HTMLMaker maker;
    maker.setPath(path);
    if(!maker.run(id_mark))
        qDebug() << "FALLLSEEEEEEEE!!!!!";

}

void MainWindow::saveToFile(QString content, QString fileName)
{
    QFile file(QDir::currentPath() + "/out/" + fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << content;
    file.close();
}

void MainWindow::on_manCopy_clicked()
{
    QModelIndex index = ui->ManualsListView->currentIndex();
    if(!index.isValid())
        return;

    copyMan = true;
    copyIdMan = manModel->index(index.row(), 1).data().toInt();
    ui->statusBar->showMessage(tr("Скопировано!"));
}
