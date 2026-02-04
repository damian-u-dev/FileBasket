#include "AppModel.h"

AppModel::AppModel()
{
    tabs.push_back({"Default", {}}); //TODO: PlaceHolder;
}

Tab& AppModel::activeTab()
{
    return tabs[currentTab];
}

qsizetype AppModel::getSizeTabs() const
{
    return tabs.size();
}

void AppModel::setCurrentTab(int index)
{
    if(index < 0 || index > getSizeTabs())
        return;

    currentTab = index;
}
