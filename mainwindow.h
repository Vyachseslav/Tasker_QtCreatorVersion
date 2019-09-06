#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    /// Загрузить список задач.
    void Loading();
    void Edit(); // Htlfrnbhjdfyb


    ~MainWindow();

private slots:
    //void on_pushButton_clicked();

    void on_btnAdd_clicked();

    void on_btnRemove_clicked();

    void on_btnEdit_clicked();
    /// Обновить список задач.
    void Refresh();

    void on_btnSolve_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);
    void LoadAllTaskToday(); // Загрузить все задачи на сегодня.
    void LoadAllTask(); // Загрузить все задачи.
    void LoadTaskToday(); // Загрузить задачи на сегодня, которые не были решены.
    void ExportToCSV(); // Экспорт в CSV-файл.
    void AboutProgram(); // Окно о программе.
    void ClearDatabse(); // Очистка базы данных.

    void on_lineEdit_returnPressed();

    void on_txtSearch_returnPressed();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
