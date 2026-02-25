#include <borealis.hpp>
#include <cstdlib>
#include <string>
#include <iostream>
#include "UI/tab_select_game.hpp"
#include "UI/Img_text_cell.hpp"
#include "UI/List_view.hpp"

#include "main_activity.hpp"

#if defined(IMPORT_MGBALIB)
#include <mgba/core/blip_buf.h>
#include <mgba/core/core.h>
#include <mgba/internal/gb/video.h>
#include <mgba/internal/gba/audio.h>
#include <mgba/internal/gba/input.h>
#include <mgba-util/gui.h>
#include <mgba-util/gui/font.h>
#include <mgba-util/gui/menu.h>
#include <mgba-util/vfs.h>
#endif


#if defined(BOREALIS_USE_OPENGL)
// Needed for the OpenGL driver to work
extern "C" unsigned int sceLibcHeapSize = 2 * 1024 * 1024;
#endif

using namespace brls::literals; // for _i18n

int main(int argc, char* argv[])
{
    // We recommend to use INFO for real apps
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-d") == 0) { // Set log level
            brls::Logger::setLogLevel(brls::LogLevel::LOG_DEBUG);
        } else if (std::strcmp(argv[i], "-o") == 0) {
            const char* path = (i + 1 < argc) ? argv[++i] : "borealis.log";
            brls::Logger::setLogOutput(std::fopen(path, "w+"));
        } else if (std::strcmp(argv[i], "-v") == 0) {
            brls::Application::enableDebuggingView(true);
        }
    }


    brls::Platform::APP_LOCALE_DEFAULT = brls::LOCALE_AUTO;

    // Init the app and i18n
    if (!brls::Application::init())
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }

    brls::Application::createWindow("beiklive/title"_i18n);

    // brls::Application::getPlatform()->setThemeVariant(brls::ThemeVariant::DARK);

    // Have the application register an action on every activity that will quit when you press BUTTON_START
    brls::Application::setGlobalQuit(false);
    brls::Application::registerXMLView("HomeMenuListView", HomeMenuListView::create);
    brls::Application::registerXMLView("Img_text_cell", Img_text_cell::create);
    
    
    auto mainActivity = new MainActivity();
    
    brls::Application::pushActivity(mainActivity);
    mainActivity->InitActivity();


    // Run the app
    while (brls::Application::mainLoop())
        ;

    // Exit
    return EXIT_SUCCESS;
}

#ifdef __WINRT__

#include <borealis/core/main.hpp>
#endif
