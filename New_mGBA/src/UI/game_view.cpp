#include "UI/game_view.hpp"
#include <borealis.hpp>
#include <cmath>

GameView::GameView(std::string gameName) : GameView()
{
    brls::Logger::debug("GameView constructor with gameName called: {}", gameName);
    m_gameName = std::move(gameName);
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
    brls::Logger::debug("GameView destructor called");
}


void GameView::initialize()
{
    brls::Logger::debug("GameView::initialize() - starting initialization");
}

void GameView::draw(NVGcontext* vg, float x, float y, float width, float height,
                    brls::Style style, brls::FrameContext* ctx)
{
    // 清屏（可选，因为全屏渲染会覆盖整个屏幕）
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
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
