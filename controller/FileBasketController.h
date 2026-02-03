#ifndef FILEBASKETCONTROLLER_H
#define FILEBASKETCONTROLLER_H

#include <QObject>

class AppModel;

class FileBasketController : public QObject
{
private:
    AppModel& model;
public:
    FileBasketController(AppModel& model, QObject* parent = nullptr);
};

#endif // FILEBASKETCONTROLLER_H
