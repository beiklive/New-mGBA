#include "UI/tab_select_game.hpp"

using namespace brls::literals;  // for _i18n

SelectGameTab::SelectGameTab()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/mgba_xml/tabs/select_game.xml");

    // notify->registerClickAction([](...){
    //     std::string notification = NOTIFICATIONS[std::rand() % NOTIFICATIONS.size()];
    //     brls::Application::notify(notification);
    //     return true;
    // });
}

brls::View* SelectGameTab::create()
{
    return new SelectGameTab();
}
