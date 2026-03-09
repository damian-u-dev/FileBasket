#ifndef PERSISTENCESERVICE_H
#define PERSISTENCESERVICE_H

#include <QString>

class AppModel;

class PersistenceService
{
public:
    void save(const AppModel& model);
    void load(AppModel& model);

private:
    QString sessionPath() const;
};

#endif // PERSISTENCESERVICE_H
