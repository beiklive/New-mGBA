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

    void loadGame();
    void glInit(void);
    void glDeinit(void);
    void _drawFrame();

    void draw(NVGcontext* vg, float x, float y, float width, float height,
              brls::Style style, brls::FrameContext* ctx);
  private:
    std::string m_gamePath;
      bool m_gameLoaded = false;

    void mInputMapInit(struct mInputMap* map, const struct mInputPlatformInfo* info);



      // opengl
      GLuint program;
      GLuint vbo;
      GLuint vao;
      GLuint pbo;
      GLuint copyFbo;
      GLuint texLocation;
      GLuint dimsLocation;
      GLuint insizeLocation;
      GLuint colorLocation;
      GLuint tex;
      GLuint oldTex;
      static color_t* frameBuffer;
      void _clearScreen();

};

};
