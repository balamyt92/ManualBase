#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRelationalTableModel>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QFile>

#include "manualdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_SectionsListView_clicked(const QModelIndex &index);
    void on_MarksListView_clicked(const QModelIndex &index);
    void on_markDel_clicked();
    void on_markAdd_clicked();

    void on_sectionAdd_clicked();

    void on_sectionDel_clicked();

    void on_ModelAdd_clicked();

    void on_modelDel_clicked();

    void on_ModelsListView_clicked(const QModelIndex &index);

    void on_ManualsListView_doubleClicked(const QModelIndex &index);

    void on_manAdd_clicked();

    void on_manDel_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow              *ui;
    QSqlTableModel              *sectionModel;
    QSqlTableModel              *markModel;
    QSqlTableModel              *modelModel;
    QSqlRelationalTableModel    *manModel;
    QString                     currenSection;
    QString                     currentMark;
    QString                     currentModel;
    QString                     rootURL;

    void saveToFile(QString content, QString fileName);

};

#endif // MAINWINDOW_H
