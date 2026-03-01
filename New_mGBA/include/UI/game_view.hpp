#pragma once

#include <borealis.hpp>
#include <glad/glad.h>

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

  private:
    GLuint m_textureId = 0;
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_program = 0;
    
    // 状态标志
    bool m_is_initialized = false;
    int m_screen_width = 0;
    int m_screen_height = 0;
    
    // Uniform locations
    GLint m_texture_uniform = -1;
    GLint m_position_location = -1;
    GLint m_time_uniform = -1;
    GLint m_resolution_uniform = -1;
    
    // 动画时间
    float m_time = 0.0f;
    
    void initialize();
    void cleanupOpenGL();
    void checkShader(GLuint handle);
    bool useCoreShaders();
    void checkAndInitialize(int width, int height);
    void checkAndUpdateScale(int width, int height);
    void bindTexture();
};

