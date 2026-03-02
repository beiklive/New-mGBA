#include "Game/GameRuntime.hpp"

MGBA_EXPORT const struct mInputPlatformInfo GBAInputInfo = {
    .platformName = "gba",
    .keyId = (const char*[]) {
        "A",
        "B",
        "Select",
        "Start",
        "Right",
        "Left",
        "Up",
        "Down",
        "R",
        "L"
    },
    .nKeys = GBA_KEY_MAX,
    .hat = {
        .up = GBA_KEY_UP,
        .right = GBA_KEY_RIGHT,
        .down = GBA_KEY_DOWN,
        .left = GBA_KEY_LEFT
    }
};

static void _updateLoading(size_t read, size_t size, void* context)
{
    // brls::Logger::debug("Loading ROM: {}% ({} / {})", (read * 100) / size, read, size);
}
namespace beiklive {
    


GameRuntime::GameRuntime(const std::string& gamePath)
{
    m_gamePath = gamePath;
    InitRomCore();
    
}

GameRuntime::~GameRuntime()
{
}

void GameRuntime::InitRomCore()
{
    bool found = false;
    // 使用 mCoreFind 查找核心并初始化
    gameRunner->core = mCoreFind(m_gamePath.c_str());
    if (gameRunner->core) {
        gameRunner->core->init(gameRunner->core);
        mCoreInitConfig(gameRunner->core, gameRunner->port);
        mInputMapInit(&gameRunner->core->inputMap, &GBAInputInfo);
        struct VFile* rom = mDirectorySetOpenPath(&gameRunner->core->dirs, m_gamePath.c_str(), gameRunner->core->isROM);
        
        found = mCorePreloadVFCB(gameRunner->core, rom, _updateLoading, gameRunner);
        if(!found) {
            if(rom) {
                rom->close(rom);
            }
            gameRunner->core->deinit(gameRunner->core);
            brls::Logger::error("Failed to load ROM: {}", m_gamePath);
            return;
        }
        brls::Logger::debug("ROM loaded successfully: {}", m_gamePath);

        if (gameRunner->core->platform(gameRunner->core) == mPLATFORM_GBA) {
		    gameRunner->core->setPeripheral(gameRunner->core, mPERIPH_GBA_LUMINANCE, &gameRunner->luminanceSource.d);
            gameRunner->gameFile.type = mPLATFORM_GBA;
        }else{
            gameRunner->gameFile.type = mPLATFORM_GB;
        }

        // 读取配置
        mCoreLoadForeignConfig(gameRunner->core, &gameRunner->config);
        mCoreAutoloadSave(gameRunner->core);  // 读取存档文件
	    mCoreAutoloadCheats(gameRunner->core); // 读取金手指文件

        gameRunner->core->reset(gameRunner->core);

    } else {
        brls::Logger::error("Failed to initialize ROM core for game: {}", m_gamePath);
    }
}

void GameRuntime::draw(NVGcontext* vg, float x, float y, float width, float height, brls::Style style, brls::FrameContext* ctx)
{
        // 清屏（可选，因为全屏渲染会覆盖整个屏幕）
    gameRunner->core->runFrame(gameRunner->core);
}

void GameRuntime::mInputMapInit(struct mInputMap* map, const struct mInputPlatformInfo* info) {
	map->maps = 0;
	map->numMaps = 0;
	map->info = info;
}


};
