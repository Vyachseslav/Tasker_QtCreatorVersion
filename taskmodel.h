#ifndef TASKMODEL_H
#define TASKMODEL_H

#include "QString"

class TaskModel
{
public:
    TaskModel();
    // Конструктор.
    TaskModel(int id, QString name, QString date, int isSolved);
    // Id задачи.
    int Id;
    // Название задачи.
    QString Name;
    // Дата выполнения задачи.
    QString DateOn;
    // Задача выполнена.
    int IsSolved;
    bool IsSolvedBool;

};

#endif // TASKMODEL_H
