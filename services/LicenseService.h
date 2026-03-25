#ifndef LICENSESERVICE_H
#define LICENSESERVICE_H

class QString;

class LicenseService
{
public:
    static bool isPro();
    static bool activate(const QString& key);

private:
    static bool validateKey(const QString& key);
};

#endif // LICENSESERVICE_H
