#include "UpdateCheckerService.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#ifndef APP_BUILD_NUMBER
#define APP_BUILD_NUMBER 0
#endif

UpdateCheckerService::UpdateCheckerService(QObject *parent)
    : QObject{parent}
{
    manager = new QNetworkAccessManager(this);
}

void UpdateCheckerService::checkForUpdates()
{
    QUrl url("https://api.github.com/repos/damian-u-dev/FileBasket/releases/latest");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::UserAgentHeader, "FileBasket");

    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]()
        {
            if(reply->error() != QNetworkReply::NoError)
            {
                reply->deleteLater();
                return;
            }

            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject obj = doc.object();

            QString latestTag = obj["tag_name"].toString();

            latestTag.remove('v');
            const int latestVersion = latestTag.toInt();

            if(latestVersion > APP_BUILD_NUMBER)
            {
                QJsonArray assets = obj["assets"].toArray();
                if(!assets.isEmpty())
                {
                    QString downloadUrl =
                        assets[0].toObject()["browser_download_url"].toString();

                    emit updateAvailable(latestTag, downloadUrl);
                }
            }
            else
            {
                emit noUpdate();
            }
            reply->deleteLater();
        });
}
