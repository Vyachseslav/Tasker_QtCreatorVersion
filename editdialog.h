#ifndef EDITDIALOGTWO_H
#define EDITDIALOGTWO_H

#include <QDialog>
#include <QString>

namespace Ui {
class EditDialogTwo;
}

class EditDialogTwo : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialogTwo(QWidget *parent = 0);
    explicit EditDialogTwo(int id, QString name, QString date, QWidget *parent = 0);
    ~EditDialogTwo();

private slots:
    void on_buttonBox_accepted();
    // Получить строку Sql-запроса для добавления задачи.
    QString GetSqlQueryForEditTask(int id, QString name, QString date);
    // Выполнить sql-запрос к БД.
    void EditTask(QString* sqlQuery);

private:
    Ui::EditDialogTwo *ui;
    int ID;
};

#endif // EDITDIALOGTWO_H
