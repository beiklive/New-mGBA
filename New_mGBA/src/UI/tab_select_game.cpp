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

    auto dismissAction = [](View* view) {
        brls::Logger::info("Clicked on btn");
        return true;
    };

    select_file->registerClickAction(dismissAction);


    // select_file->setImage("img/tiles.png");
    // select_file->setTitle("beiklive/select/file"_i18n);

    // select_recent->setImage("img/tiles.png");
    // select_recent->setTitle("beiklive/select/recent"_i18n);

    // select_favorites->setImage("img/tiles.png");
    // select_favorites->setTitle("beiklive/select/favorites"_i18n);
    applyBackTheme(brls::Application::getPlatform()->getThemeVariant());

}

brls::View* SelectGameTab::create()
{
    return new SelectGameTab();
}


void SelectGameTab::applyBackTheme(brls::ThemeVariant theme)
{
    switch (theme)
    {
        case brls::ThemeVariant::LIGHT:
            select_file->setImage("img/ui/folder.png");
            select_recent->setImage("img/ui/history.png");
            select_favorites->setImage("img/ui/bookmark.png");
            break;
        case brls::ThemeVariant::DARK:
            select_file->setImage("img/ui/folder_dark.png");
            select_recent->setImage("img/ui/history_dark.png");
            select_favorites->setImage("img/ui/bookmark_dark.png");
            break;
    }
}
