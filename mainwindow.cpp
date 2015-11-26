#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("base.db3");
    if(!db.open()) {
        QString err = tr("Не могу подключиться к базе! <br>") + db.lastError().text();
        QMessageBox::critical(this, tr("Ошибка!"), err);
    }

    QSqlQuery query("PRAGMA foreign_keys = ON");

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
    ui->groupBox_4->setEnabled(false);
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
