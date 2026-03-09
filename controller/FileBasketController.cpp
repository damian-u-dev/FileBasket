#include "FileBasketController.h"
#include "../model/AppModel.h"
#include "../services/FileOperationService.h"

#include <QVector>
#include <QMessageBox>
#include <QInputDialog>

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
    bool success = FileOperationService::runExplorerOperation(paths, targetDir, OperationType::Move);

    if(!success)
        return;

    QMessageBox::StandardButton answer = QMessageBox::question(nullptr,
                    "Confirm removing",
                    "All the files were successfully moved. Remove them from the current tab?");
    if(answer == QMessageBox::Yes)
    {
        model.removeFilesFromActiveTab(selectedIndices);
    }
    else
    {
        model.updatePaths(selectedIndices, targetDir);
    }
}

void FileBasketController::setActiveTab(int index)
{
    model.setActiveTab(index);
}

void FileBasketController::moveItems(const QVector<int>& indices)
{
    if(indices.isEmpty())
        return;

    QStringList names = model.getTabNames();
    int currentIndex = model.getIndexActiveTab();
    names.removeAt(currentIndex);

    bool ok = false;
    QString chosen = QInputDialog::getItem(
        nullptr,
        "Select Tab",
        "Move to:",
        names,
        0,
        false,
        &ok);

    if(!ok || chosen.isEmpty())
        return;

    int indexOldTab = model.getIndexActiveTab();
    int indexSelectedTab = model.getTabIndexByName(chosen);

    if(indexSelectedTab < 0 || indexSelectedTab >= model.getAmountTabs())
        return;

    const QString oldTab = model.getTabName(indexOldTab);
    const QString newTab = model.getTabName(indexSelectedTab);

    //Preparing message
    QString elem = (indices.size() > 1)
                ? QString("%1 files").arg(indices.size())
                : QString("%1 file").arg(indices.size());

    QString readyMessage = QStringLiteral("Do you want to move %1 from \"%2\" into \"%3\"?")
                .arg(elem, oldTab, newTab);

    QMessageBox::StandardButton answer =
        QMessageBox::question(nullptr, "Confirm moving", readyMessage);


    if(answer == QMessageBox::Yes)
    {
        int expectedMoved = indices.size();
        int totalMoved = model.moveFilesFromActiveTab(indices, indexSelectedTab);

        QString elem = (expectedMoved > 1)
                           ? QString("%1 out of %2 files were moved to").arg(totalMoved).arg(expectedMoved)
                           : QString("%1 out of %2 file was moved to").arg(totalMoved).arg(expectedMoved);

        QMessageBox::information(nullptr, "Info"
                ,QString("%1 \"%3\"").arg(elem).arg(newTab));
    }
}

void FileBasketController::createTab(const QString& name)
{
    if(!model.createTab(name))
    {
        QMessageBox::warning(nullptr,
                             "Error",
                             "Tab with this name already exists");
    }
}
