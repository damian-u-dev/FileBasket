#include "FileBasketController.h"
#include "../model/AppModel.h"
#include "../services/FileOperationService.h"

#include <QVector>
#include <QMessageBox>

FileBasketController::FileBasketController(AppModel& model, QObject* parent)
    : QObject(parent),
      model(model)
{ }

void FileBasketController::addFiles(const QStringList& paths)
{
    model.addFilesToActiveTab(paths);
}

void FileBasketController::removeItems(const QVector<int>& indices)
{
    if(indices.isEmpty())
        return;

    const int count = indices.size();

    const QString text =
        (count == 1) ? QStringLiteral("Do you want to remove the selected item?")
        : QStringLiteral("Do you want to remove %1 items?").arg(count);

    QMessageBox::StandardButton result
            = QMessageBox::question(
            nullptr,
            QStringLiteral("Confirm removing"),
            text,
            QMessageBox::Yes | QMessageBox::No);

    if(result != QMessageBox::Yes)
        return;

    model.removeFilesFromActiveTab(indices);
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
