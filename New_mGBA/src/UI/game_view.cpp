#include "UI/game_view.hpp"
#include <borealis.hpp>
#include <cmath>

GameView::GameView(std::string gameName) : GameView()
{
    brls::Logger::debug("GameView constructor with gameName called: {}", gameName);
    m_gameName = std::move(gameName);
    initialize();
}

GameView::GameView()
{
    brls::Logger::debug("GameView constructor called");
    setFocusable(true);
    setHideHighlight(true);
    brls::Logger::debug("GameView constructed, initialization deferred to first draw");
}

GameView::~GameView()
{
    delete m_gameRuntime;
    brls::Logger::debug("GameView destructor called");
}


void GameView::initialize()
{
    m_gameRuntime = new beiklive::GameRuntime(m_gameName);
    m_gameRuntime->loadGame();
    brls::Logger::debug("GameView::initialize() - GameRuntime created for game: {}", m_gameName);
}
void GameView::draw(NVGcontext* vg, float x, float y, float width, float height,
                    brls::Style style, brls::FrameContext* ctx)
{

    if (!m_gameRuntime) {
        brls::Logger::error("GameRuntime not initialized, cannot draw game");
        return;
    }

    m_gameRuntime->draw(vg, x, y, width, height, style, ctx);

    // 请求重绘以保持动画
    // this->invalidate();
}

void GameView::onFocusGained()
{
    brls::Logger::debug("GameView::onFocusGained() called");
    Box::onFocusGained();
}

void GameView::onFocusLost()
{
    brls::Logger::debug("GameView::onFocusLost() called");
    Box::onFocusLost();
}

void GameView::onLayout()
{
    brls::Logger::debug("GameView::onLayout() called");
    Box::onLayout();
}
