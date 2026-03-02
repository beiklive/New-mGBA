#pragma once

#include <mgba-util/circle-buffer.h>
#include <mgba-util/gui.h>
#include <mgba-util/gui/font.h>
#include <mgba-util/gui/menu.h>
#include <mgba-util/vfs.h>
#include <mgba/core/blip_buf.h>
#include <mgba/core/core.h>
#include <mgba/core/rewind.h>
#include <mgba/gba/interface.h>
#include <mgba/internal/gb/video.h>
#include <mgba/internal/gba/audio.h>
#include <mgba/internal/gba/input.h>

#include "Utils/ConfigManager.hpp"
#include "Utils/fileUtils.hpp"
#include "Utils/strUtils.hpp"

namespace beiklive
{

struct Gamefile
{
    // 游戏文件的路径
    std::string path;
    // 游戏文件名称
    std::string name;
    // 游戏文件显示名称
    std::string displayName;
    // 游戏文件类型（例如：GBA、GB）
    mPlatform type = mPLATFORM_NONE;
    // 游戏输出分辨率
    int width;
    int height;
};
struct mGUIRunnerLux {
	struct GBALuminanceSource d;
	int luxLevel;
};

// 游戏运行时环境
struct GameRunner
{
    struct mCore* core;
    struct mCoreConfig config;
    beiklive::ConfigManager* settingConfig;
    beiklive::ConfigManager* nameMappingConfig;
    Gamefile gameFile;
    std::string currentPath;
    
    struct mGUIRunnerLux luminanceSource; // 亮度信息来源


    struct mCoreRewindContext rewind; // 倒带上下文
    bool rewinding; // 当前是否正在倒带
    bool rewindEnabled; // 倒带功能是否启用
    bool rewindMuteEnabled; // 倒带静音功能是否启用
    int rewindBufferSize; // 倒带缓冲区大小（帧数）
    int rewindSaveInterval; // 保存间隔（每N帧保存一次）
    unsigned rewindFrames; // 已保存的帧数

    // 用于倒带状态显示
    bool rewindPaused; // 倒带时是否暂停
    int rewindShowStatus; // 倒带状态显示

    const char* port;
    float fps;
    int64_t lastFpsCheck;
    int32_t totalDelta;
    struct CircleBuffer fpsBuffer;
};


};

// 全局配置管理器实例
extern beiklive::ConfigManager* SettingManager;
extern beiklive::ConfigManager* NameMappingManager;
extern beiklive::GameRunner* gameRunner;