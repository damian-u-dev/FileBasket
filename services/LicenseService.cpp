#include "LicenseService.h"

#include <QSettings>
#include <QCryptographicHash>

static const QString key = "Lorem ipsum";

bool LicenseService::isPro()
{
    QSettings settings;
    return settings.value("pro", false).toBool();
}

bool LicenseService::activate(const QString& key)
{
    if(validateKey(key))
    {
        QSettings settings;
        settings.setValue("pro", true);
        return true;
    }
    return false;
}

bool LicenseService::validateKey(const QString& key)
{
    QStringList parts = key.split('-');
    if(parts.size() != 2)
        return false;

    QString user = parts[0];
    QString hash = parts[1];

    QByteArray data = (user + key).toUtf8();
    QByteArray expectedHash =
        QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();

    return hash == expectedHash.left(10);
}
