#include "FileOperationService.h"

//Windows API
#include <windows.h>
#include <shellapi.h>
//Windows API

#include <string>

static std::wstring buildDoubleNullString(const QStringList& paths)
{
    std::wstring result;

    for(const QString& path : paths)
    {
        std::wstring w = path.toStdWString();
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

    //Files will be copied and moved in one operation.
    std::wstring from = buildDoubleNullString(paths);

    std::wstring to = targetDir.toStdWString();
    to.push_back(L'\0');
    to.push_back(L'\0');

    SHFILEOPSTRUCTW op = {0};
    op.wFunc  = (type == OperationType::Copy) ? FO_COPY : FO_MOVE;
    op.pFrom  = from.c_str();
    op.pTo    = to.c_str();
    op.fFlags = FOF_NOCONFIRMATION;

    int result = SHFileOperationW(&op);

    if(result != 0)
        return false;

    if(op.fAnyOperationsAborted)
        return false;

    return true;
}
