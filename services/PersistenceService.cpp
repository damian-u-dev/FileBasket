#include "PersistenceService.h"
#include "../model/AppModel.h"
#include "../Config.h"

#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QSaveFile>

QString PersistenceService::sessionPath() const
{
    QString dir	=
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir().mkpath(dir);

    return dir + "/session.json";
}

void PersistenceService::save(const AppModel& model)
{
    QJsonObject root;
    root["currentTab"] = model.getIndexActiveTab();

    QJsonArray tabsArray;

    for(const auto& tab : model.getTabs())
    {
    	QJsonObject tabObj;

    	tabObj["name"] = tab.name;

    	QJsonArray filesArray;

    	for(const auto& file : tab.files)
    	{
    		QJsonObject fileObj;

    		fileObj["path"] = file.path;
    		fileObj["size"] = static_cast<qint64>(file.size);

    		filesArray.append(fileObj);
    	}

    	tabObj["files"] = filesArray;

    	tabsArray.append(tabObj);
    }

    root["tabs"] = tabsArray;

    QJsonDocument doc(root);
    QSaveFile file(sessionPath());
    
    if(!file.open(QIODevice::WriteOnly))
    	return;

    file.write(doc.toJson(QJsonDocument::Indented));
    file.commit();
}

void PersistenceService::load(AppModel& model)
{
    QFile file(sessionPath());

    if(!file.exists())
    {
        model.createTab(Config::DefaultTabName);
        return;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        model.createTab(Config::DefaultTabName);
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc =
        QJsonDocument::fromJson(data, &error);

    if(error.error != QJsonParseError::NoError)
    {
        model.createTab(Config::DefaultTabName);
        return;
    }

    if(!doc.isObject())
    {
        model.createTab(Config::DefaultTabName);
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray tabsArray = root["tabs"].toArray();

    if(tabsArray.isEmpty())
    {
        model.createTab(Config::DefaultTabName);
        return;
    }

    for(const QJsonValue& tabVal : std::as_const(tabsArray))
    {
        QJsonObject tabObj = tabVal.toObject();

        QString name = tabObj["name"].toString();

        if(name.isEmpty())
            name = "Tab";

        model.createTab(name);

        QJsonArray filesArray =
            tabObj["files"].toArray();

        for(const QJsonValue& fileVal : std::as_const(filesArray))
        {
            QJsonObject fileObj = fileVal.toObject();
            QString path = fileObj["path"].toString();
            qint64 size =  fileObj["size"].toInteger();

            QFileInfo info(path);

            if(!info.exists())
                continue;

            model.addFileToTab(name, path, size);
        }
    }

    int currentTab =
        root["currentTab"].toInt();

    if(currentTab >= 0 &&
        currentTab < model.getAmountTabs())
    {
        model.setActiveTab(currentTab);
    }
}
