#include "mainwindow.h"
#include "database.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    rootURL = "http://www.autoinfo24.ru/rukovodstva-po-remontu/";

    DataBase *db = new DataBase(this);
    if(!db->connectToBase()) {
        QMessageBox::critical(this, tr("Ошибка!"), db->lastErr());
        this->setEnabled(false);
    }
    else {
        // Разделы
        sectionModel = new QSqlTableModel(this);
        sectionModel->setTable("sections");
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
        //markModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        //ui->MarksListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->MarksListView->setModel(markModel);
        ui->MarksListView->setModelColumn(1);
        ui->groupBox_2->setEnabled(false);

        // Модели
        modelModel = new QSqlTableModel(this);
        modelModel->setTable("models");
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
        manModel->setRelation(0, QSqlRelation("manual", "ID", "Name, ID"));
        ui->ManualsListView->setModel(manModel);
        ui->ManualsListView->setModelColumn(0);
        ui->ManualsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->groupBox_4->setEnabled(false);
    }
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
            QFile::remove(QDir::currentPath() + "/foto/" + id_man + ".jpg");
            QFile::copy(md->getFoto(), QDir::currentPath() + "/foto/" + id_man + ".jpg");
            md->setPathToFile("foto/" + id_man + ".jpg");
        }
    }
    delete md;
}

void MainWindow::on_manAdd_clicked()
{
    ManualDialog *md = new ManualDialog(this);
    md->addManual(currentModel);
    if(md->exec() == QDialog::Accepted)
    {
        md->saveManual();
        manModel->select();
        QFile::copy(md->getFoto(), QDir::currentPath() + "/foto/" + md->getIDLastAddManual() + ".jpg");

        md->setPathToFile("foto/" + md->getIDLastAddManual() + ".jpg");
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

        QFile::remove(QDir::currentPath() + "/foto/" + id_man + ".jpg");
    }
}

#include <QTextEdit>
void MainWindow::on_pushButton_clicked()
{
    QModelIndex index = ui->MarksListView->currentIndex();
    if(!index.isValid())
        return;
    QString id_mark = markModel->index(index.row(), 0).data().toString();

    QFile::remove(QDir::currentPath() + "/out/");
    QDir out(QDir::currentPath());
    out.mkdir("out");
    out.cd("out");

    // make menu models
    QTextEdit *browser = new QTextEdit(this);
    browser->setGeometry(300, 300, 400, 500);
    browser->setPlainText(makeMenu(id_mark));
    browser->show();

    // make pages
    // make foto folder

}

#include <QDialog>
#include <QVBoxLayout>
#include <QTableView>
#include <QDialogButtonBox>
QString MainWindow::makeMenu(QString id_mark)
{
    QStringList modelList;
    QString menu;
    QSqlQuery query;
    query.prepare("SELECT Name FROM models WHERE ID_Mark=" + id_mark);
    query.exec();
    while (query.next()) {
        modelList << query.value(0).toString();
    }

    query.prepare("SELECT Name, ID_Sections FROM marks WHERE ID=" + id_mark);
    query.exec();
    query.next();
    QString markName = query.value(0).toString();
    QString sectionName = query.value(1).toString();

    query.prepare("SELECT Name FROM sections WHERE ID=" + sectionName);
    query.exec();
    query.next();
    sectionName = query.value(0).toString();

    QDialog *dialog = new QDialog(this);
    QTableView *view = new QTableView(dialog);
    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok
                                                 | QDialogButtonBox::Cancel, dialog);
    connect(box, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(box, SIGNAL(rejected()), dialog, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(view);
    layout->addWidget(box);
    dialog->setLayout(layout);

    query.prepare("DROP TABLE tmp");
    query.exec();

    query.prepare("CREATE TABLE tmp(Name TEXT, URL TEXT)");
    query.exec();


    QSqlTableModel *model = new QSqlTableModel(dialog);
    model->setTable("tmp");
    model->select();
    for (int i = 0; i < modelList.count(); ++i) {
        model->insertRow(model->rowCount());
        QString name = modelList.at(i);
        model->setData(model->index(i, 0), name);
        model->setData(model->index(i, 1), rootURL + sectionName.toLower().replace(" ", "-") + "/"
                                                   + markName.toLower().replace(" ", "-") + "/"
                                                   + name.toLower().replace(" ", "-") + "/");
        model->submitAll();
    }
    view->setModel(model);


    QString elements;
    if(dialog->exec() == QDialog::Accepted) {
        // make menu
        QFile file(":menu_element.html");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        QString element = in.readAll();

        for (int i = 0; i < model->rowCount(); ++i) {
            QString tmp = element;
            tmp.replace("%MODEL_NAME%", model->index(i, 0).data().toString());
            tmp.replace("%URL_TO_MODEL%", model->index(i, 1).data().toString());
            elements.append(tmp);
        }
        file.close();

        QFile file2(":menu_template.html");
        file2.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in2(&file2);
        menu = in2.readAll();
        menu.replace("%MENU%", elements);
    }
    return menu;
}
