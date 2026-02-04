#include "FileBasketController.h"
#include "../model/AppModel.h"

#include <QVector>

FileBasketController::FileBasketController(AppModel& model, QObject* parent)
    : QObject(parent),
      model(model)
{ }

void FileBasketController::addFiles(const QStringList& paths)
{

}

void FileBasketController::copyTo(const QString& targetDir, const QVector<int>& selectedIndices)
{

}

void FileBasketController::moveTo(const QString& targetDir, const QVector<int>& selectedIndices)
{

}

void FileBasketController::setActiveTab(int index)
{
    model.setCurrentTab(index);
}
