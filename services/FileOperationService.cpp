//Windows API
#include <windows.h>
#include <shellapi.h>
//Windows API

#include <string>
#include <QDebug>
#include <QStringLiteral>
#include <QDir>

#include "FileOperationService.h"

static std::wstring buildDoubleNullString(const QStringList& paths)
{
    std::wstring result;

    for(const QString& path : paths)
    {
        QString native = QDir::toNativeSeparators(path);
        std::wstring w = native.toStdWString();
        result.append(w);
        result.push_back(L'\0');
    }
    result.push_back(L'\0');
    return result;
}

bool FileOperationService::runExplorerOperation(
    const QStringList& paths,
    const QString& targetDir,
    OperationType type)
{
    if(paths.isEmpty() || targetDir.isEmpty())
        return false;

    qInfo() << "Copying/Moving files...\n";

    //NOTE: Files will be copied and moved in one operation.
    std::wstring from = buildDoubleNullString(paths);

    QString nativeTarget = QDir::toNativeSeparators(targetDir);
    std::wstring to = nativeTarget.toStdWString();
    to.push_back(L'\0');

    SHFILEOPSTRUCTW op = {0};
    op.wFunc  = (type == OperationType::Copy) ? FO_COPY : FO_MOVE;
    op.pFrom  = from.c_str();
    op.pTo    = to.c_str();
    op.fFlags = FOF_NOCONFIRMATION;

    int result = SHFileOperationW(&op);

    if(result != 0)
    {
        qWarning() << QStringLiteral("Copy/Move ended with error code: %1").arg(result);
        return false;
    }

    if(op.fAnyOperationsAborted)
    {
        qInfo() << "Copy/Move aborted";
        return false;
    }

    const QString& operation = (type == OperationType::Copy) ? "FO_COPY" : "FO_MOVE";

    qInfo() << "Copy/Move successfully done";
    qInfo() << QStringLiteral("Total files: %1").arg(paths.size());
    qInfo() << QStringLiteral("Target directory: %1").arg(targetDir);
    qInfo() << QStringLiteral("Operation: %1").arg(operation);

    return true;
}
