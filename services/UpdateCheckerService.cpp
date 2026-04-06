#include "UpdateCheckerService.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define APP_VERSION "1.5"

UpdateCheckerService::UpdateCheckerService(QObject *parent)
    : QObject{parent}
{
    manager = new QNetworkAccessManager(this);
}

void UpdateCheckerService::checkForUpdates()
{
    QUrl url("https://api.github.com/repos/damian-u-dev/FileBasket/releases/latest");
    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);

    connect(reply, QNetworkReply::finished, this, [=]()
        {
            if(reply->error() != QNetworkReply::NoError)
                return;

            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject obj = doc.object();

            QString latestVersion = obj["tag_name"].toString();

            if(latestVersion > APP_VERSION)
            {
                QJsonArray assets = obj["assets"].toArray();
                if(!assets.isEmpty())
                {
                    QString downloadUrl =
                        assets[0].toObject()["browser_download_url"].toString();

                    emit updateAvailable(latestVersion, downloadUrl);
                }
            }
            else
            {
                emit noUpdate();
            }
            reply->deleteLater();
        });
}
