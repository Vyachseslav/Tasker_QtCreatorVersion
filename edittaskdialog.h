#ifndef EDITTASKDIALOG_H
#define EDITTASKDIALOG_H

#include <QDialog>

namespace Ui {
class EditTaskDialog;
}

class EditTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditTaskDialog(QWidget *parent = 0);
    explicit EditTaskDialog(int id, QString name, QString date, int isSolved, QWidget *parent = 0);
    QString GetSqlQueryForEditTask(int id, QString name, QString date, int isSolved);
    void EditTask(QString* sqlQuery);
    ~EditTaskDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::EditTaskDialog *ui;
    /// Id выбранной задачи.
    int ID;
};

#endif // EDITTASKDIALOG_H
