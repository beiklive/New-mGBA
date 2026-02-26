#pragma once

#include <borealis.hpp>

class GameView : public brls::Box
{
  public:
    GameView();
    ~GameView();

    void draw(NVGcontext* vg, float x, float y, float width, float height,
              brls::Style style, brls::FrameContext* ctx) override;
    void onFocusGained() override;
    void onFocusLost() override;
    void onLayout() override;


};

