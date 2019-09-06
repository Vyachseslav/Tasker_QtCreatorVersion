#include "editdialog.h"
#include "ui_editdialogtwo.h"
#include <QString>
#include <QDate>
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"

EditDialogTwo::EditDialogTwo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialogTwo)
{
    ui->setupUi(this);
}

EditDialogTwo::EditDialogTwo(int id, QString name, QString date, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialogTwo)
{
    ui->setupUi(this);

    ui->label->setText("*Название задачи");
    ui->label_2->setText("*Дата выполнения задачи");
    setWindowTitle("Редактирование задачи");

    ID = id;
    ui->lineEdit->setText(name);
    QDate selectedDate = QDate::fromString(date,"dd.MM.yyyy");
    ui->calendarWidget->setSelectedDate(selectedDate);
}

EditDialogTwo::~EditDialogTwo()
{
    delete ui;
}

void EditDialogTwo::on_buttonBox_accepted()
{
    QString taskName = ui->lineEdit->text(); // Название задачи.
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString("dd.MM.yyyy");

    QDate selectedDate = ui->calendarWidget->selectedDate();
    dateTimeString = selectedDate.toString("dd.MM.yyyy");

    QString sqlQuery = GetSqlQueryForEditTask(ID, taskName, dateTimeString);
    EditTask(&sqlQuery);
}

// Получить строку Sql для добавления задачи.
QString EditDialogTwo::GetSqlQueryForEditTask(int id, QString name, QString date)
{
    QString sqlQuery = "update Tasks set Name='";
    sqlQuery = sqlQuery.append(name);
    sqlQuery = sqlQuery.append("', DateOn='");
    sqlQuery = sqlQuery.append(date);
    sqlQuery = sqlQuery.append("' where Id=");
    sqlQuery = sqlQuery.append(QString::number(id));
    return sqlQuery;
}

// Выполнить Sql-запрос в БД.
void EditDialogTwo::EditTask(QString* sqlQuery)
{
    //Подключаем базу данных
   QSqlDatabase db;
   db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName("TaskerDatabase.db3");
   db.open();

   //Осуществляем запрос
   QSqlQuery query;
   query.exec(*sqlQuery);
}
