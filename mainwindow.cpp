#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"
#include "QStandardItemModel"
#include "QStandardItem"
#include "adddialog.h"
#include "editdialog.h"
#include<QMessageBox>
#include <QList>
#include "taskmodel.h"
#include <QAction>
#include <QMenuBar>
#include "editdialog.h"
#include <QDateTime>
#include "edittaskdialog.h"
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QStatusBar>
#include <QCheckBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "aboutdialog.h"
#include <QFileDialog>
#include <QTextStream>

QAction *taskToday;
QAction *taskAllToday;
QAction *taskAll;

// Перечисление возможных видов списка задач.
enum TaskView
{
    TodayIsNotSolved = 0, // задачи на сегодня, которые не решены.
    TodayAll, // все задачи на сегодня.
    AllTasks, // все задачи.
    AllTasksIsNotSolved // все задачи, которые не решены.
} viewsOfTask;

TaskView _selectedTaskView = TodayAll;

QStandardItemModel *model;
//QList<TaskModel> *TaskList;

QSqlDatabase db;

QLabel *labelCount;
QLabel *labelViewStatus;

// Открыть соединение с базой.
void OpenSqlConnection()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("TaskerDatabase.db3");
    db.open();
}

// Получить Sql-запрос в зависимости от вида списка задач.
QString GetSqlRequest(TaskView view)
{
    QString result = "";
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString("dd.MM.yyyy");

    switch (view)
    {
       case 2:
         result = "SELECT Id, Name, DateOn, IsSolved FROM Tasks";
         labelViewStatus->setText("Все задачи");
         taskAll->setChecked(true);
         taskAllToday->setChecked(false);
         taskToday->setChecked(false);
         break;
       case 1:
        result = "SELECT Id, Name, DateOn, IsSolved FROM Tasks where DateOn='" + dateTimeString + "'";
        labelViewStatus->setText("Все задачи на сегодня");
        taskAll->setChecked(false);
        taskAllToday->setChecked(true);
        taskToday->setChecked(false);
        break;
       case 0:
        result = "SELECT Id, Name, DateOn, IsSolved FROM Tasks where (DateOn='" + dateTimeString + "' AND IsSolved=0)";
        labelViewStatus->setText("Все открытые задачи на сегодня");
        taskAll->setChecked(false);
        taskAllToday->setChecked(false);
        taskToday->setChecked(true);
        break;
       case 3:
        result = "SELECT Id, Name, DateOn, IsSolved FROM Tasks where IsSolved=0";
        labelViewStatus->setText("Нерешеные задачи");
        break;
    }
    return result;
}

// Редактировать задачу.
void MainWindow::Edit()// Редактировать задачу.
{
    QString id = model->data(model->index(ui->tableView->currentIndex().row(), 0)).toString();
    QString name = model->data(model->index(ui->tableView->currentIndex().row(), 1)).toString();
    QString date = model->data(model->index(ui->tableView->currentIndex().row(), 2)).toString();
    QString isSolved = model->data(model->index(ui->tableView->currentIndex().row(), 3)).toString();

    EditTaskDialog dialog(id.toInt(), name, date, isSolved.toInt());
    int result = dialog.exec();
    if (result == 1)
        Loading();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Tasker - Ваши задачи на сегодня");

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Запрет редактирования.
    OpenSqlConnection(); // Открыть соединение с БД.

    /* Создание текстового меню программы. */
    QMenu *file = menuBar()->addMenu("Файл");
    QMenu *exportMenu = new QMenu("Экспорт");
    QMenu *edit = menuBar()->addMenu("Правка");
    QMenu *view = menuBar()->addMenu("Вид");
    QMenu *filter = menuBar()->addMenu("Фильтрация");
    QMenu *help = menuBar()->addMenu("Помощь");

    QAction *solveTask = new QAction(tr("Решить задачу"), this);
    solveTask->setShortcut(tr("Ctrl+Shift+D"));
    connect(solveTask, SIGNAL(triggered()), this, SLOT(on_btnSolve_clicked()));
    file->addAction(solveTask);

    file->addMenu(exportMenu);
    QAction *exportCSV = new QAction(tr("Экспорт в csv"), this);
    connect(exportCSV, SIGNAL(triggered()), this, SLOT(ExportToCSV()));
    exportMenu->addAction(exportCSV);

    file->addSeparator();

    QAction *clearDatabase = new QAction(tr("Очистить базу"), this);
    connect(clearDatabase, SIGNAL(triggered()), this, SLOT(ClearDatabse()));
    file->addAction(clearDatabase);

    QAction *refresh = new QAction(tr("Обновить"), this);
    refresh->setShortcut(tr("F5"));
    connect(refresh, SIGNAL(triggered()), this, SLOT(Refresh()));
    edit->addAction(refresh);

    edit->addSeparator();

    QAction *newFile = new QAction(tr("Новый"), this);
    newFile->setShortcut(tr("Ctrl+N"));
    connect(newFile, SIGNAL(triggered()), this, SLOT(on_btnAdd_clicked()));
    edit->addAction(newFile);

    QAction *openFile = new QAction(tr("Открыть"), this);
    openFile->setShortcut(tr("Ctrl+O"));
    connect(openFile, SIGNAL(triggered()), this, SLOT(on_btnEdit_clicked()));
    edit->addAction(openFile);

    QAction *removeFile = new QAction(tr("Удалить"), this);
    removeFile->setShortcut(tr("Delete"));
    connect(removeFile, SIGNAL(triggered()), this, SLOT(on_btnRemove_clicked()));
    edit->addAction(removeFile);

    taskToday = new QAction(tr("Задачи на сегодня"), this);
    taskToday->setToolTip("Показать задачи на текущую дату, которые не были решены");
    taskToday->setCheckable(true);
    connect(taskToday, SIGNAL(triggered()), this, SLOT(LoadTaskToday()));
    view->addAction(taskToday);

    taskAllToday = new QAction(tr("Все задачи на сегодня"), this);
    taskAllToday->setToolTip("Показать все задачи на текущую дату");
    taskAllToday->setCheckable(true);
    taskAllToday->setChecked(true);
    connect(taskAllToday, SIGNAL(triggered()), this, SLOT(LoadAllTaskToday()));
    view->addAction(taskAllToday);

    taskAll = new QAction(tr("Все задачи"), this);
    taskAll->setToolTip("Показать все задачи");
    taskAll->setCheckable(true);
    connect(taskAll, SIGNAL(triggered()), this, SLOT(LoadAllTask()));
    view->addAction(taskAll);

    QAction *filterSettings = new QAction(tr("Установить фильтр"), this);
    filterSettings->setEnabled(false);
    filter->addAction(filterSettings);

    QAction *disableFilter = new QAction(tr("Отменить фильтр"), this);
    disableFilter->setEnabled(false);
    filter->addAction(disableFilter);

    QAction *aboutProgram = new QAction(tr("О программе"), this);
    connect(aboutProgram, SIGNAL(triggered()), this, SLOT(AboutProgram()));
    help->addAction(aboutProgram);

    /* Создание labels в StatusBar. */
    labelCount = new QLabel(this);
    statusBar()->addWidget(labelCount);
    labelViewStatus = new QLabel(this);
    statusBar()->addWidget(labelViewStatus);

    /* Загрузка данных. */
    Loading();
}

MainWindow::~MainWindow()
{
    delete ui;
    db.close();
}

/// Обновить данные.
void MainWindow::Refresh()
{
    //_selectedTaskView = TodayIsNotSolved;
    Loading();
    ui->tableView->selectRow(0);
}

void MainWindow::Loading()
{
    model = new QStandardItemModel;

    //Заголовки столбцов
    QStringList horizontalHeader;
    horizontalHeader.append("#");
    horizontalHeader.append("Описание задачи");
    horizontalHeader.append("Дата выполнения");
    horizontalHeader.append("Выполнено (1 или 0)");
    horizontalHeader.append("");

    model->setHorizontalHeaderLabels(horizontalHeader);

    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString("dd.MM.yyyy");

    //Осуществляем запрос
    QString sqlQuery = GetSqlRequest(_selectedTaskView);

    QSqlQuery query;
    query.exec(sqlQuery);

    //Выводим значения из запроса
    int i=0;
    while (query.next())
    {
       QString _id = query.value(0).toString();
       QString name = query.value(1).toString();
       QString age = query.value(2).toString();
       QString isSolved = query.value(3).toString();
       bool solve = (isSolved == 1) ? true : false;

       model->setItem(i, 0, new QStandardItem(_id));
       model->setItem(i, 1, new QStandardItem(name));
       model->setItem(i, 2, new QStandardItem(age));
       model->setItem(i, 3, new QStandardItem(isSolved));
//       QCheckBox *check = new QCheckBox(this);
//       check->setChecked(solve);
//       //model->setItem(i, 4, new QStandardItem(check));
//       model->setData(model->index(i, 4), check);
       model->setData(model->index(i, 4), isSolved.toInt(), Qt::CheckStateRole);

       // Create an element, which will serve as a checkbox
//       QTableWidgetItem *item = new QTableWidgetItem();
//       item->data(Qt::CheckStateRole);
//       /* Check on the status of odd if an odd device,
//        * exhibiting state of the checkbox in the Checked, Unchecked otherwise
//        * */
//       if(solve){
//            item->setCheckState(Qt::Checked);
//       } else {
//            item->setCheckState(Qt::Unchecked);
//       }
//       // Set the checkbox in the second column
//       model->setItem(i, 4, new QStandardItem(item));

       i++;
    }

    ui->tableView->setModel(model);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(3);
    //ui->tableView->resizeRowsToContents();
    ui->tableView->rowHeight(18);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->selectRow(0);

    /* Устанавливаем ширину столбцов. */
    ui->tableView->setColumnWidth(1, 600);
    ui->tableView->setColumnWidth(2, 140);
    ui->tableView->setColumnWidth(4, 50);

    labelCount->setText("Кол-во записей: " + QString::number(model->rowCount()));
}

void MainWindow::on_btnAdd_clicked()
{
    AddDialog dialog;
    int result = dialog.exec();
    if (result == 1) // Нажата кнопка "OK".
    {
        Loading();
    }
}

void MainWindow::on_btnEdit_clicked()
{
    Edit();
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    Edit();
}

void MainWindow::on_btnRemove_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Вы действительно хотите удалить выбранную задачу?");
    //msgBox.setInformativeText("Вы действительно хотите удалить выбранную задачу?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch(ret){
       case QMessageBox::Cancel:
        return;
    }

    QString id = model->data(model->index(ui->tableView->currentIndex().row(), 0)).toString();
    QString sqlQuery = "delete from Tasks where Id=";
    sqlQuery = sqlQuery.append(id);

    //Осуществляем запрос
    QSqlQuery query;
    bool flag = query.exec(sqlQuery);

    if (flag)
        Loading();
}

void MainWindow::on_btnSolve_clicked()
{
    QString id = model->data(model->index(ui->tableView->currentIndex().row(), 0)).toString();
    QString sqlQuery = "update Tasks set IsSolved=1 where Id=";
    sqlQuery = sqlQuery.append(id);

    //Осуществляем запрос
    QSqlQuery query;
    bool flag = query.exec(sqlQuery);

    if (flag)
        Loading();
}

void MainWindow::LoadAllTask()
{
    _selectedTaskView = AllTasks;
    Loading();
}

void MainWindow::LoadAllTaskToday()
{
    _selectedTaskView = TodayAll;
    Loading();
}

void MainWindow::LoadTaskToday()
{
    _selectedTaskView = TodayIsNotSolved;
    Loading();
}

void MainWindow::on_lineEdit_returnPressed(){}

void MainWindow::on_txtSearch_returnPressed()
{
    QStandardItemModel *modelFind;
    int ii = 0;

        QString searchText = ui->txtSearch->text();
        if (searchText != "")
        {
            ii = 0;
            modelFind = new QStandardItemModel;
            QList<QStandardItem*> findes = model->findItems(searchText, Qt::MatchContains, 1);

            foreach (QStandardItem* item, findes) {
                modelFind->setItem(ii, item);
                //modelFind->setData(ii, item);
                ii++;
                //ui->tableView->setSelectionModel(modelFind);
            }
            //Заголовки столбцов
                    QStringList horizontalHeader;
                    horizontalHeader.append("#");
                    horizontalHeader.append("Описание задачи");
                    horizontalHeader.append("Дата выполнения");
                    horizontalHeader.append("Выполнено");

                    modelFind->setHorizontalHeaderLabels(horizontalHeader);

                    ui->tableView->setModel(modelFind);
                    ui->tableView->hideColumn(0);
                    ui->tableView->rowHeight(18);
                    ui->tableView->resizeColumnsToContents();
                    ui->tableView->selectRow(0);
            //ui->tableView->setModel(modelFind);
        }
        else
            Loading();
}

void MainWindow::ExportToCSV()
{
    QString savePath = QFileDialog::getSaveFileName(0, "Экспорт данных в csv", "", "*.csv");
    if (savePath == "")
        return;
    QFile f(savePath);

    if( f.open( QIODevice::WriteOnly ) )
    {
       QTextStream ts( &f );
       QStringList strList;

       strList << "\" \"";
       for( int c = 0; c < ui->tableView->horizontalHeader()->count(); ++c )
       {
           strList << "\""+ui->tableView->model()->headerData(c, Qt::Horizontal).toString()+"\"";
       }
       ts << strList.join( ";" )+"\n";

       for( int r = 0; r < ui->tableView->verticalHeader()->count(); ++r )
       {
                strList.clear();
                strList << "\""+ui->tableView->model()->headerData(r, Qt::Vertical).toString()+"\"";
                for( int c = 0; c < ui->tableView->horizontalHeader()->count(); ++c )
                {
                    strList << "\""+ui->tableView->model()->data(ui->tableView->model()->index(r, c), Qt::DisplayRole).toString()+"\"";
                }
                ts << strList.join( ";" )+"\n";
       }
       f.close();
     }

    QMessageBox *message;
    message->setText("Экспорт в файл '" + savePath + "' завершен!");
    message->exec();
}

void MainWindow::AboutProgram()
{
    AboutDialog dialog;
    dialog.exec();
}

void MainWindow::ClearDatabse()
{
    QMessageBox msgBox;
    msgBox.setText("Вы действительно хотите полностью очистить базу данных?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch(ret){
       case QMessageBox::Cancel:
        return;
    }

    QString sqlQuery = "delete from Tasks";

    //Осуществляем запрос
    QSqlQuery query;
    bool flag = query.exec(sqlQuery);

    if (flag)
        Loading();
}
