#include "XMLUI/StartPageView.hpp"

#include "UI/game_view.hpp"
#include "XMLUI/Pages/AppPage.hpp"

StartPageView::StartPageView()
{
    if (!gameRunner)
    {
        return;
    }
    if (SettingManager->Contains(KEY_UI_START_PAGE))
    {
        int start_page_index = *gameRunner->settingConfig->Get(KEY_UI_START_PAGE)->AsInt();
        if (start_page_index == 0) // APP 页面
        {
            AppPage* app_page = new AppPage();
            app_page->addGame({ "/mGBA/roms/haogou.gba", "好狗狗星系", "romfs:/img/thumb/209.png" });
            app_page->addGame({ "/mGBA/roms/gba/jiezou.gba", "节奏天国", "romfs:/img/thumb/209.png" });
            app_page->addGame({ "/mGBA/roms/gba/Mother 3.gba", "地球冒险3", "romfs:/img/thumb/210.png" });
            app_page->addGame({ "/mGBA/roms/gba/MuChangWuYu.gba", "牧场物语", "" });
            app_page->addGame({ "/mGBA/roms/gba/jiezou.gba", "节奏天国", "romfs:/img/thumb/211.png" });
            app_page->addGame({ "/mGBA/roms/gba/Mother 3.gba", "地球冒险3", "romfs:/img/thumb/212.png" });
            app_page->addGame({ "/mGBA/roms/gba/MuChangWuYu.gba", "牧场物语", "" });
            app_page->addGame({ "/mGBA/roms/gba/jiezou.gba", "节奏天国", "romfs:/img/thumb/213.png" });
            app_page->addGame({ "/mGBA/roms/gba/Mother 3.gba", "地球冒险3", "romfs:/img/thumb/214.png" });
            app_page->addGame({ "/mGBA/roms/gba/MuChangWuYu.gba", "牧场物语", "" });

#ifdef __SWITCH__
            app_page->onGameSelected = [](const GameEntry& e)
            {
                auto* frame = new brls::AppletFrame(new GameView(e.path));
                frame->setHeaderVisibility(brls::Visibility::GONE);
                frame->setFooterVisibility(brls::Visibility::GONE);
                frame->setBackground(brls::ViewBackground::NONE);
                brls::Application::pushActivity(new brls::Activity(frame));
            };
#else
            app_page->onGameSelected = [this](const GameEntry& e)
            {
                bklog::debug("Game Selected: {} {} {}", e.path, e.title, e.cover);
            };
#endif
            addView(app_page);
        }
        else if (start_page_index == 1) // 文件列表界面
        {
        }
        bklog::debug("Startup Page: {}", start_page_index);
    }
}

StartPageView::~StartPageView()
{
}

void StartPageView::onFocusGained()
{
    Box::onFocusGained();
}

void StartPageView::onFocusLost()
{
    Box::onFocusLost();
}

void StartPageView::onLayout()
{
    Box::onLayout();
}

brls::View* StartPageView::create()
{
    return new StartPageView();
}