#include "../model/AppModel.h"

#include <QStringList>
#include <QtTest>

// add necessary includes here

class AppModelTest : public QObject
{
    Q_OBJECT

public:
    AppModelTest();
    ~AppModelTest();

private slots:
    void test_addFilesToActiveTab();
};

AppModelTest::AppModelTest() {}

AppModelTest::~AppModelTest() {}

void AppModelTest::test_addFilesToActiveTab()
{
    AppModel model;

    QStringList paths =
    {
        "C:/appverifUI.dll",
        "C:/vfcompat.dll",
    };

    model.addFilesToActiveTab(paths);

    QCOMPARE(model.activeTab().files.size(), 2);
    QCOMPARE(model.activeTab().files[0].path, paths[0]);
    QCOMPARE(model.activeTab().files[1].path, paths[1]);
}

QTEST_APPLESS_MAIN(AppModelTest)
#include "tst_appmodeltest.moc"
