#include "UI/game_view.hpp"
#include <glad/glad.h>
#include <borealis.hpp>
#include <cmath>

// Core 版本着色器 - 动态彩虹渐变
const char* vertexShaderSourceCore = R"(
    #version 330 core
    layout (location = 0) in vec2 position;
    out vec2 fragCoord;
    
    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
        fragCoord = (position + 1.0) * 0.5; // 转换到 0-1 范围
    }
)";

const char* fragmentShaderSourceCore = R"(
    #version 330 core
    in vec2 fragCoord;
    out vec4 FragColor;
    
    uniform float time;
    uniform vec2 resolution;
    
    vec3 hsv2rgb(vec3 c) {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }
    
    void main()
    {
        vec2 uv = fragCoord;
        
        // 基于位置和时间计算色相
        float hue = uv.x + time * 0.1;
        hue = fract(hue); // 保持在 0-1 范围
        
        // 基于 y 坐标和时间调整饱和度和亮度
        float sat = 0.8 + 0.2 * sin(uv.y * 3.14159 + time * 0.5);
        float val = 0.6 + 0.4 * cos(uv.y * 3.14159 - time * 0.3);
        
        vec3 color = hsv2rgb(vec3(hue, sat, val));
        FragColor = vec4(color, 1.0);
    }
)";

// ES 版本着色器 - 动态彩虹渐变
const char* vertexShaderSourceES = R"(
    attribute vec2 position;
    varying vec2 fragCoord;
    
    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
        fragCoord = (position + 1.0) * 0.5;
    }
)";

const char* fragmentShaderSourceES = R"(
    precision mediump float;
    
    varying vec2 fragCoord;
    uniform float time;
    uniform vec2 resolution;
    
    vec3 hsv2rgb(vec3 c) {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }
    
    void main()
    {
        vec2 uv = fragCoord;
        
        float hue = uv.x + time * 0.1;
        hue = fract(hue);
        
        float sat = 0.8 + 0.2 * sin(uv.y * 3.14159 + time * 0.5);
        float val = 0.6 + 0.4 * cos(uv.y * 3.14159 - time * 0.3);
        
        vec3 color = hsv2rgb(vec3(hue, sat, val));
        gl_FragColor = vec4(color, 1.0);
    }
)";

// 全屏四边形顶点（NDC 坐标）
static const float vertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f
};

GameView::GameView()
{
    brls::Logger::debug("GameView constructor called");

    setFocusable(true);
    setHideHighlight(true);
    // setBackground(brls::ViewBackground::BACKDROP);
    brls::Logger::debug("GameView constructed, initialization deferred to first draw");
}

GameView::~GameView()
{
    brls::Logger::debug("GameView destructor called");
    cleanupOpenGL();
    brls::Logger::debug("GameView cleaned up");
}

void GameView::checkShader(GLuint handle)
{
    GLint success = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    
    brls::Logger::info("GL: GL_COMPILE_STATUS: {}", success);
    
    if (!success) {
        GLint length = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
        
        if (length > 0) {
            char* buffer = (char*)malloc(length);
            glGetShaderInfoLog(handle, length, &length, buffer);
            brls::Logger::error("GL: Compile shader error: {}", buffer);
            free(buffer);
        }
    }
}

bool GameView::useCoreShaders()
{
    const char* version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    brls::Logger::debug("GL: GLSL Version: {}", version);
    return version[0] == '3' || version[0] == '4';
}

void GameView::initialize()
{
    brls::Logger::debug("GameView::initialize() - starting initialization");
    
    m_program = glCreateProgram();
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    
    bool use_gl_core = useCoreShaders();
    brls::Logger::debug("GL: Using {} shaders", use_gl_core ? "Core" : "ES");
    
    // 编译顶点着色器
    const char* vertSrc = use_gl_core ? vertexShaderSourceCore : vertexShaderSourceES;
    glShaderSource(vert, 1, &vertSrc, nullptr);
    glCompileShader(vert);
    checkShader(vert);
    
    // 编译片段着色器
    const char* fragSrc = use_gl_core ? fragmentShaderSourceCore : fragmentShaderSourceES;
    glShaderSource(frag, 1, &fragSrc, nullptr);
    glCompileShader(frag);
    checkShader(frag);
    
    // 链接程序
    glAttachShader(m_program, vert);
    glAttachShader(m_program, frag);
    glLinkProgram(m_program);
    
    // 检查链接状态
    GLint success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint length = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            char* buffer = (char*)malloc(length);
            glGetProgramInfoLog(m_program, length, &length, buffer);
            brls::Logger::error("GL: Link program error: {}", buffer);
            free(buffer);
        }
    } else {
        brls::Logger::debug("GL: Shader program linked successfully - ID: {}", m_program);
    }
    
    // 删除着色器对象
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    // 创建 VBO 和 VAO
    glGenBuffers(1, &m_VBO);
    glGenVertexArrays(1, &m_VAO);
    brls::Logger::debug("GL: VAO and VBO generated - VAO: {}, VBO: {}", m_VAO, m_VBO);
    
    // 获取 uniform 位置
    m_texture_uniform = glGetUniformLocation(m_program, "texture1");
    m_time_uniform = glGetUniformLocation(m_program, "time");
    m_resolution_uniform = glGetUniformLocation(m_program, "resolution");
    brls::Logger::debug("GL: time uniform location: {}", m_time_uniform);
    brls::Logger::debug("GL: resolution uniform location: {}", m_resolution_uniform);
    
    brls::Logger::debug("GameView::initialize() - initialization complete");
}

void GameView::bindTexture()
{
    // 不再需要纹理，注释掉或保持空实现
    brls::Logger::debug("GameView::bindTexture() - using procedural shader, no texture needed");
}

void GameView::checkAndInitialize(int width, int height)
{
    if (!m_is_initialized) {
        brls::Logger::info("GL: Init with width: {}, height: {}", width, height);
        
        m_is_initialized = true;
        initialize();  // ← 在这里调用！
        
        brls::Logger::info("GL: Init done");
    }
}

void GameView::checkAndUpdateScale(int width, int height)
{
    bool needsUpdate = (m_screen_width != width) || 
                       (m_screen_height != height) ||
                       !useCoreShaders(); // Dirty fix for GLES
    
    if (needsUpdate) {
        brls::Logger::debug("GL: Updating scale - old: {}x{}, new: {}x{}", 
                           m_screen_width, m_screen_height, width, height);
        
        m_screen_width = width;
        m_screen_height = height;
        
        // 上传顶点数据
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        // 配置顶点属性
        m_position_location = glGetAttribLocation(m_program, "position");
        glEnableVertexAttribArray(m_position_location);
        glVertexAttribPointer(m_position_location, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        brls::Logger::debug("GL: Position attribute location: {}", m_position_location);
        
        // 设置分辨率 uniform
        glUniform2f(m_resolution_uniform, (float)width, (float)height);
        brls::Logger::debug("GL: Resolution set to: {}x{}", width, height);
        
        brls::Logger::debug("GL: Scale update complete");
    }
}

void GameView::cleanupOpenGL()
{
    brls::Logger::debug("GameView::cleanupOpenGL() - starting cleanup");
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        brls::Logger::debug("VAO deleted");
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        brls::Logger::debug("VBO deleted");
    }
    if (m_textureId) {
        glDeleteTextures(1, &m_textureId);
        brls::Logger::debug("Texture deleted");
    }
    if (m_program) {
        glDeleteProgram(m_program);
        brls::Logger::debug("Shader program deleted");
    }
    brls::Logger::debug("GameView::cleanupOpenGL() - cleanup complete");
}

void GameView::draw(NVGcontext* vg, float x, float y, float width, float height,
                    brls::Style style, brls::FrameContext* ctx)
{
    // 更新时间（每帧递增）
    m_time += 0.016f; // 假设 60fps，约 16ms 每帧
    
    brls::Logger::debug("GameView::draw() called - pos: ({}, {}), size: ({}x{}), time: {}", 
                       x, y, width, height, m_time);
    
    // 初始化检查
    checkAndInitialize((int)width, (int)height);
    
    // 绑定 VAO
    glBindVertexArray(m_VAO);
    
    // 使用着色器程序
    glUseProgram(m_program);
    checkAndUpdateScale((int)width, (int)height);
    
    // 设置时间 uniform
    glUniform1f(m_time_uniform, m_time);
    
    // 清屏（可选，因为全屏渲染会覆盖整个屏幕）
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    brls::Logger::debug("GL: Screen cleared, time uniform set to: {}", m_time);
    
    // 绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    brls::Logger::debug("GL: glDrawArrays executed");
    
    // 重置状态
    glUseProgram(0);
    
    brls::Logger::debug("GameView dynamic gradient rendered");
    
    // 请求重绘以保持动画
    this->invalidate();
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
