#include "edittaskdialog.h"
#include "ui_edittaskdialog.h"
#include <QString>
#include <QDate>
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"
#include "taskmodel.h"

//TaskModel _taskModel;

EditTaskDialog::EditTaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTaskDialog)
{
    ui->setupUi(this);
}

EditTaskDialog::EditTaskDialog(int id, QString name, QString date, int isSolved, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTaskDialog)
{
    ui->setupUi(this);

    ID = id;
//    _taskModel.Id = id;
//    _taskModel.Name = name;
//    _taskModel.DateOn = date;
//    _taskModel.IsSolved = isSolved;
    bool solve = (isSolved == 1) ? true : false;

//    _taskModel.IsSolvedBool = solve;

    ui->txtTaskName->setText(name);
    QDate selectedDate = QDate::fromString(date,"dd.MM.yyyy");
    ui->calendarWidget->setSelectedDate(selectedDate);
    ui->checkIsSolved->setChecked(solve);
}

EditTaskDialog::~EditTaskDialog()
{
    delete ui;
}

void EditTaskDialog::on_buttonBox_accepted()
{
    QString taskName = ui->txtTaskName->text(); // Название задачи.
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString("dd.MM.yyyy");

    QDate selectedDate = ui->calendarWidget->selectedDate();
    dateTimeString = selectedDate.toString("dd.MM.yyyy");

    int flag = (ui->checkIsSolved->isChecked()) ? 1 : 0;

//    _taskModel.Id = ID;
//    _taskModel.Name = taskName;
//    _taskModel.DateOn = dateTimeString;
//    _taskModel.IsSolved = flag;

    QString sqlQuery = GetSqlQueryForEditTask(ID, taskName, dateTimeString, flag);
    EditTask(&sqlQuery);
}

// Получить строку Sql для добавления задачи.
QString EditTaskDialog::GetSqlQueryForEditTask(int id, QString name, QString date, int isSolved)
{
    QString sqlQuery = "update Tasks set Name='";
    sqlQuery = sqlQuery.append(name);
    sqlQuery = sqlQuery.append("', DateOn='");
    sqlQuery = sqlQuery.append(date);
    sqlQuery = sqlQuery.append("', IsSolved=");
    sqlQuery = sqlQuery.append(QString::number(isSolved));
    sqlQuery = sqlQuery.append(" where Id=");
    sqlQuery = sqlQuery.append(QString::number(id));
    return sqlQuery;
}

// Выполнить Sql-запрос в БД.
void EditTaskDialog::EditTask(QString* sqlQuery)
{
    //Подключаем базу данных
   QSqlDatabase db;
   db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName("TaskerDatabase.db3");
   db.open();

   //Осуществляем запрос
   QSqlQuery query;
   query.exec(*sqlQuery);

   //db.close();
}
