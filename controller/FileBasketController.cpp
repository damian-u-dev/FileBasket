#include "FileBasketController.h"
#include "../model/AppModel.h"
#include "../services/FileOperationService.h"

#include <QVector>

FileBasketController::FileBasketController(AppModel& model, QObject* parent)
    : QObject(parent),
      model(model)
{ }

void FileBasketController::addFiles(const QStringList& paths)
{
    model.addFilesToActiveTab(paths);
}

void FileBasketController::copyTo(const QString& targetDir, const QVector<int>& selectedIndices)
{
    if(selectedIndices.isEmpty())
        return;

    const Tab& tab = model.activeTab();
    QStringList paths;

    for(int index : selectedIndices)
    {
        if(index >= 0 && index < tab.files.size())
            paths << tab.files[index].path;
    }
    FileOperationService::runExplorerOperation(paths, targetDir, OperationType::Copy);
}

void FileBasketController::moveTo(const QString& targetDir, const QVector<int>& selectedIndices)
{
    if(selectedIndices.isEmpty())
        return;

    const Tab& tab = model.activeTab();
    QStringList paths;

    for(int index : selectedIndices)
    {
        if(index >= 0 && index < tab.files.size())
            paths << tab.files[index].path;
    }
    FileOperationService::runExplorerOperation(paths, targetDir, OperationType::Move);
}

void FileBasketController::setActiveTab(int index)
{
    model.setCurrentTab(index);
}
