#include "UI/game_view.hpp"

GameView::GameView()
{

}

GameView::~GameView()
{
}

void GameView::draw(NVGcontext* vg, float x, float y, float width, float height,
                    brls::Style style, brls::FrameContext* ctx)
{
    // Call the base class draw method to draw the background and border
    Box::draw(vg, x, y, width, height, style, ctx);

    // Here you would add code to draw the game screen using the NVGcontext
    // For example, you could use nvgDrawImage to draw the game framebuffer
}

void GameView::onFocusGained()
{
    Box::onFocusGained();

    // Handle focus gained event, e.g., start the game or resume it
}

void GameView::onFocusLost()
{
    Box::onFocusLost();

    // Handle focus lost event, e.g., pause the game
}

void GameView::onLayout()
{
    Box::onLayout();
    // Handle layout changes, e.g., adjust the game screen size or position
}
