#include "adddialog.h"
#include "ui_adddialog.h"
#include <QMessageBox>
#include <QString>
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"
#include "QDateTime"

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::on_buttonBox_accepted()
{
    QString taskName = ui->lineEdit->text(); // Название задачи.
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString("dd.MM.yyyy");

    QDate selectedDate = ui->calendarWidget->selectedDate();
    dateTimeString = selectedDate.toString("dd.MM.yyyy");

    QString sqlQuery = GetSqlQueryForAddTask(taskName, dateTimeString);
    AddNewTask(&sqlQuery);
}

// Получить строку Sql для добавления задачи.
QString AddDialog::GetSqlQueryForAddTask(QString name, QString date)
{
    QString sqlQuery = "insert into Tasks (Name, DateOn) values ('";
    sqlQuery = sqlQuery.append(name);
    sqlQuery = sqlQuery.append("', '");
    sqlQuery = sqlQuery.append(date);
    sqlQuery = sqlQuery.append("');");
    return sqlQuery;
}

// Выполнить Sql-запрос в БД.
void AddDialog::AddNewTask(QString* sqlQuery)
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
