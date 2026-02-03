#include "AppModel.h"

AppModel::AppModel()
{
    tabs.push_back({"Default", {}}); //TODO: PlaceHolder;
}

Tab& AppModel::activeTab()
{
    return tabs[currentTab];
}
