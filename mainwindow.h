#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QMessageBox>

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
    void on_SectionsListView_doubleClicked(const QModelIndex &index);

    void on_MarksListView_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QSqlDatabase        *db;
    QSqlTableModel      *sectionModel;
    QSqlTableModel      *markModel;
    QSqlTableModel      *modelModel;
    QSqlTableModel      *manModel;
};

#endif // MAINWINDOW_H
