#ifndef UPDATECHECKERSERVICE_H
#define UPDATECHECKERSERVICE_H

#include <QObject>
#include <QNetworkAccessManager>

class UpdateCheckerService : public QObject
{
    Q_OBJECT
public:
    explicit UpdateCheckerService(QObject *parent = nullptr);
    void checkForUpdates();

signals:
    void updateAvailable(QString version, QString url);
    void noUpdate();

private:
    QNetworkAccessManager* manager;
};

#endif // UPDATECHECKERSERVICE_H
