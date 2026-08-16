#ifndef FILEOPERATIONSERVICE_H
#define FILEOPERATIONSERVICE_H

#include <QStringList>
#include <QObject>

enum class OperationType
{
    Copy,
    Move
};

class FileOperationService : public QObject
{
    Q_OBJECT
public:
    bool runExplorerOperation(
        const QStringList& paths,
        const QString& targetDir,
        OperationType type);
};

#endif // FILEOPERATIONSERVICE_H
