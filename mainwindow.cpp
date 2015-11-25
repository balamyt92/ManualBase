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

//    QSqlQuery query("PRAGMA foreign_keys = ON");
//    qDebug() << query.lastError().text();

    // Разделы
    sectionModel = new QSqlTableModel(this);
    sectionModel->setTable("sections");
    if(!sectionModel->select()) {
        QString err = tr("Не могу считать разделы! <br>") + sectionModel->lastError().text();
        QMessageBox::critical(this, tr("Ошибка!"), err);
    }
    ui->SectionsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->SectionsListView->setModel(sectionModel);
    ui->SectionsListView->setModelColumn(1);

    // Марки
    markModel = new QSqlTableModel(this);
    markModel->setTable("marks");
    ui->MarksListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->MarksListView->setModel(markModel);
    ui->MarksListView->setModelColumn(1);

    // Модели
    modelModel = new QSqlTableModel(this);
    modelModel->setTable("models");
    ui->ModelsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ModelsListView->setModel(modelModel);
    ui->ModelsListView->setModelColumn(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_SectionsListView_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    QString id = sectionModel->index(index.row(), 0).data().toString();
    markModel->setFilter("ID_Sections=" + id);
    if(!markModel->select()) {
        QString err = tr("Не могу считать список марок! <br>") + markModel->lastError().text();
        QMessageBox::critical(this, tr("Ошибка!"), err);
    }
}

void MainWindow::on_MarksListView_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    QString id = markModel->index(index.row(), 0).data().toString();
    modelModel->setFilter("ID_Mark=" + id);
    if(!modelModel->select()) {
        QString err = tr("Не могу считать список моделей! <br>") + modelModel->lastError().text();
        QMessageBox::critical(this, tr("Ошибка!"), err);
    }
}
