#include "taskmodel.h"

TaskModel::TaskModel()
{

}

// Конструктор.
TaskModel::TaskModel(int id, QString name, QString date, int isSolved)
{
    this->Id = id;
    this->Name = name;
    this->DateOn = date;
    this->IsSolved = isSolved;
    if (isSolved == 1)
    {
        this->IsSolvedBool = true;
    }
    else
    {
        this->IsSolvedBool = false;
    }
}
