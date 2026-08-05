#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QSet>

namespace Config
{
    inline const QString DefaultTabName = QStringLiteral("New tab");

    inline const QSet<QString> DynamicThumbnailExtensions =
    {
        QStringLiteral("png"),
        QStringLiteral("jpg"),
        QStringLiteral("jpeg"),
    };

    inline bool supportsDynamicThumbnail(const QString& extension)
    {
        return DynamicThumbnailExtensions.contains(extension);
    }
}


#endif // CONFIG_H
