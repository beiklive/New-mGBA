#pragma once

#include "common.hpp"
#include <borealis.hpp>
#include <glad/glad.h>
namespace beiklive
{

class GameRuntime
{
  public:
    GameRuntime() = default;
    GameRuntime(const std::string& gamePath);
    ~GameRuntime();

    void InitRomCore();

    void draw(NVGcontext* vg, float x, float y, float width, float height,
              brls::Style style, brls::FrameContext* ctx);
  private:
    std::string m_gamePath;


    void mInputMapInit(struct mInputMap* map, const struct mInputPlatformInfo* info);

};

};
