#ifndef FILEOPERATIONSERVICE_H
#define FILEOPERATIONSERVICE_H

#include <QStringList>

enum class OperationType
{
    Copy,
    Move
};

class FileOperationService
{
public:
    static bool runExplorerOperation(
        const QStringList& paths,
        const QString& targetDir,
        OperationType type);
};

#endif // FILEOPERATIONSERVICE_H
