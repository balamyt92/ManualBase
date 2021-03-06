#include "htmlmaker.h"

HTMLMaker::HTMLMaker(QObject *parent) : QObject(parent)
{

}

void HTMLMaker::setPath(QString _path)
{
    this->path = _path;
}

bool HTMLMaker::run(QString _id_mark)
{
    if(_id_mark.isEmpty()) { return false; }
    this->id_mark = _id_mark;

    QFile file(":menu_template.html");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    this->menu_template = in.readAll();
    file.close();

    QFile file2(":menu_element.html");
    file2.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in2(&file2);
    this->menu_elem = in2.readAll();
    file2.close();

    QFile file3(":manual_template.html");
    file3.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in3(&file3);
    this->man_template = in3.readAll();
    file3.close();

    QString menu = "";
    QSqlQuery query;
    query.prepare("DROP TABLE tmp");
    query.exec();


    QFile file4(":style.css");
    file4.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in4(&file4);
    // если у нас нет моделей то менюшка не нужна
    query.prepare("SELECT COUNT(*) FROM models WHERE ID_Mark=" + id_mark);
    query.exec();
    query.next();
    bool HAVE_models = query.value(0).toBool();
    if(HAVE_models) {
        menu = in4.readAll() + makeMenu();
        file4.close();
        if(menu.isEmpty()) { qDebug() << "Ошибка при генерации меню"; return false;}
    } else {
        // сохраняем в меню только стили без генерации кнопок
        menu = in4.readAll();
    }

    QFile file5(":man_last.html");
    file5.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in5(&file5);
    QString last_template = in5.readAll();
    file5.close();

    // получаем имя марки
    query.prepare("SELECT Name FROM marks WHERE ID=" + this->id_mark);
    query.exec();
    query.next();

    QString generel_page = menu + "<h1 class=\"western\" style=\"text-align: center;\"> <em><strong><font style=\"font-size: 16px;\">Руководства по эксплуатации, обслуживанию и ремонту " + query.value(0).toString() + "</font></strong></em></h1>";;

    // если есть модели то генерим странички под них
    if(HAVE_models) {
        query.prepare("SELECT ID, Name FROM models WHERE ID_Mark=" + this->id_mark +
                      " ORDER BY models.Name");
        if(!query.exec()) { qDebug() << "query"; return false; }
        while(query.next()) {
            QString page = makePage(query.value(0).toString(), id_mark);
            page = page.replace("href=\"http://www.autoinfo24.ru/upload/images/img/not_image.jpg\"", "href=\"#\"");
            generel_page += page;
            QString tmp = menu + "<h1 class=\"western\" style=\"text-align: center;\"> <em><strong><font style=\"font-size: 16px;\">Руководства по эксплуатации, обслуживанию и ремонту " + query.value(1).toString() + "</font></strong></em></h1>";
            page = tmp.replace("<b>"+query.value(1).toString(),
                                "<b style=\"color:red;\">"+query.value(1).toString()) + page;
            QFile file(path + "/" + query.value(1).toString() + ".html");
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { return false; }
            QTextStream out(&file);
            out << page + last_template;
            file.close();
        }
    } else {
        QString page = makePage("0", id_mark);
        page = page.replace("href=\"http://www.autoinfo24.ru/upload/images/img/not_image.jpg\"", "href=\"#\"");
        generel_page += page;
    }

    QFile file1(path + "/general.html");
    if (!file1.open(QIODevice::WriteOnly | QIODevice::Text)) { return false; }
    QTextStream out1(&file1);
    out1 << generel_page + last_template;
    file1.close();


    query.prepare("DROP TABLE tmp");
    query.exec();
    return true;
}

QString HTMLMaker::makeMan(QList<QString> data, QString template_)
{
    if(!data.at(4).isEmpty()) {
        template_ = template_.replace("%PAY_URL%", data.at(4));
    } else {
        template_ = template_.replace("dispalay-pay", "style=\"display:none;\"");
    }

    if(!data.at(5).isEmpty()) {
        template_ = template_.replace("%DOWN_URL%", data.at(5));
        QRegExp rx("*.pdf");
        rx.setPatternSyntax(QRegExp::Wildcard);
        if(rx.exactMatch(data.at(5))) {
            template_ = template_.replace("target-url", "target=\"_blank\"");
        }
    } else {
        template_ = template_.replace("dispalay-down", "style=\"display:none;\"");
    }

    if(data.at(12) == "img/not_image.jpg") {
        template_ = template_.replace("<a href=\"http://www.autoinfo24.ru/upload/images/%FOTO%\"><span class=\"highslide\"><img class=\"prew_foto\" alt=\"\" src=\"/upload/images/%PREW_FOTO%\" title=\"Увеличить\"></span></a>",
                                      "<span class=\"highslide\"><img class=\"prew_foto\" alt=\"\" src=\"/upload/images/%PREW_FOTO%\" title=\"Увеличить\"></span>");
    }

    return template_.replace("%ЗАГОЛОВОК%", data.at(0))
                    .replace("%FOTO%", data.at(3))
                    .replace("%PREW_FOTO%", data.at(12))
                    .replace("%DESCRIPTION%", data.at(1))
                    .replace("%AUTOR%", data.at(2))
                    .replace("%HOME%", data.at(8))
                    .replace("%YEAR%", data.at(6))
                    .replace("%COUNTLIST%", data.at(9))
                    .replace("%FORMAT%", data.at(11))
                    .replace("%SIZE%", data.at(10));
}

QString HTMLMaker::makePage(QString id_model, QString id_mark)
{
    QString page;
    QSqlQuery query;
    query.prepare("SELECT manual.*  FROM manualtomodel "
                  "LEFT JOIN manual ON manualtomodel.ID_Man = manual.ID "
                  "WHERE ID_Model=" + id_model + " AND ID_Mark=" + id_mark +
                  " ORDER BY manual.Generetion, manual.Name");
    if(!query.exec()) goto err;

    while (query.next()) {
        QList<QString> data;
        for (int i = 1; i <= 13; ++i) {
            data << query.value(i).toString();
        }
        page += makeMan(data, this->man_template);
        if(!saveImage(query.value(0).toString())) {
            qDebug() << "Fail to copy image!";
        }
    }

    return page;
    err:
    return "";
}


#include <QDialog>
#include <QVBoxLayout>
#include <QTableView>
#include <QDialogButtonBox>
#include <QSqlTableModel>
#include <QWidget>
QString HTMLMaker::makeMenu()
{
    // clear tmp table
    QSqlQuery query;
    query.prepare("DROP TABLE tmp");
    query.exec();

    query.prepare("CREATE TABLE tmp(Name TEXT, URL TEXT)");
    query.exec();


    // create window to accepted for user
    QDialog *dialog = new QDialog(qobject_cast<QWidget*>(this->parent()));
    QTableView *view = new QTableView(dialog);
    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                 QDialogButtonBox::Cancel,
                                                 dialog);
    connect(box, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(box, SIGNAL(rejected()), dialog, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(view);
    layout->addWidget(box);
    dialog->setLayout(layout);

    QSqlTableModel *model = new QSqlTableModel(dialog);
    model->setTable("tmp");
    model->select();

    // select data
    query.prepare("SELECT models.Name, marks.Name, sections.Name FROM models "
                  "LEFT JOIN marks ON marks.ID=models.ID_Mark "
                  "LEFT JOIN sections ON sections.ID = marks.ID_Sections "
                  "WHERE models.ID_Mark=" + this->id_mark + " ORDER BY models.Name");
    if(!query.exec()) return "";
    int i = 0;
    while (query.next()) {
        model->insertRow(model->rowCount());
        model->setData(model->index(i, 0), query.value(0).toString());
        model->setData(model->index(i, 1), "http://www.autoinfo24.ru/rukovodstva-po-remontu/"
                       + query.value(2).toString().toLower().replace(" ", "-") + "/"
                       + query.value(1).toString().toLower().replace(" ", "-") + "/"
                       + query.value(0).toString().toLower().replace(" ", "-") + "/");
        model->submitAll();
        i++;
    }
    view->setModel(model);


    // make menu
    QString menu;
    if(dialog->exec() == QDialog::Accepted) {
        QString elements;
        for (int i = 0; i < model->rowCount(); ++i) {
            QString tmp = this->menu_elem;
            tmp.replace("%MODEL_NAME%", model->index(i, 0).data().toString());
            tmp.replace("%URL_TO_MODEL%", model->index(i, 1).data().toString());
            elements.append(tmp);
        }

        menu = this->menu_template;
        menu.replace("%MENU%", elements);
        return menu;
    }
    return "";
}

#include <QDir>
bool HTMLMaker::saveImage(QString id_man)
{
    QString path = QDir::currentPath() + "/out/img";
    QString inPath =  QDir::currentPath() + "/img";
    if(QFile::copy(inPath + "/" + id_man + ".jpg", path + "/" + id_man + ".jpg")) {
        if(!QFile::copy(inPath + "/prew_" + id_man + ".jpg", path + "/prew_" + id_man + ".jpg"))
            goto errImg;
    }
    else {
        errImg:
            return false;
    }
    return true;
}
