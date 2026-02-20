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

    select_file->setImage("img/tiles.png");
    select_file->setTitle("beiklive/select/file"_i18n);

    select_recent->setImage("img/tiles.png");
    select_recent->setTitle("beiklive/select/recent"_i18n);

    select_favorites->setImage("img/tiles.png");
    select_favorites->setTitle("beiklive/select/favorites"_i18n);


}

brls::View* SelectGameTab::create()
{
    return new SelectGameTab();
}
