#include "Game/GameRuntime.hpp"
// 定义自动输入的特殊ID值
#define AUTO_INPUT 0x4E585031
// 音频采样缓冲大小（512个样本）
#define SAMPLES 0x200
// 音频缓冲区数量
#define N_BUFFERS 4
// 模拟摇杆的死区阈值（用于避免输入漂移）
#define ANALOG_DEADZONE 0x4000

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
static struct mAVStream stream;    // 音视频流接口
static struct mRotationSource rotation = {0}; // 陀螺仪/加速度传感器数据
static int audioBufferActive;                  // 当前活跃的音频缓冲索引
static int enqueuedBuffers;                    // 已入队的音频缓冲数量
static float gyroZ = 0;            // 陀螺仪Z轴旋转速度
// 音频缓冲池（4个缓冲，每个大小为BUFFER_SIZE字节，4096字节对齐）
const static size_t BUFFER_SIZE = 256;
static struct mStereoSample audioBuffer[N_BUFFERS][BUFFER_SIZE / 4] __attribute__((__aligned__(0x1000)));



// 映射手柄按键到模拟器输入
static void _mapKey(struct mInputMap* map, uint32_t binding, int nativeKey, int key) {
	// 使用内置函数获取按位位置，绑定按键到输入映射
	mInputBindKey(map, binding, __builtin_ctz(nativeKey), key);
}

static void _updateLoading(size_t read, size_t size, void* context)
{
    // brls::Logger::debug("Loading ROM: {}% ({} / {})", (read * 100) / size, read, size);
}


static void _setup()
{
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_A, GBA_KEY_A);
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_B, GBA_KEY_B);
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_Plus, GBA_KEY_START);
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_Minus, GBA_KEY_SELECT);
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_Up, GBA_KEY_UP);
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_Down, GBA_KEY_DOWN);
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_Left, GBA_KEY_LEFT);
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_Right, GBA_KEY_RIGHT);
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_L, GBA_KEY_L);
	// _mapKey(&gameRunner->core->inputMap, AUTO_INPUT, HidNpadButton_R, GBA_KEY_R);

	// gameRunner->core->setPeripheral(gameRunner->core, mPERIPH_RUMBLE, &rumble.d);
	// gameRunner->core->setPeripheral(gameRunner->core, mPERIPH_ROTATION, &rotation);
	gameRunner->core->setAVStream(gameRunner->core, &stream);
	gameRunner->core->setAudioBufferSize(gameRunner->core, SAMPLES);
}
static void _gameLoaded(){

    uint32_t samplerate = 115200;
	// 计算音频比例并配置混音库
	double ratio = GBAAudioCalculateRatio(1, 60.0, 1);
	blip_set_rates(gameRunner->core->getAudioChannel(gameRunner->core, 0), gameRunner->core->frequency(gameRunner->core), samplerate * ratio);
	blip_set_rates(gameRunner->core->getAudioChannel(gameRunner->core, 1), gameRunner->core->frequency(gameRunner->core), samplerate * ratio);

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
        _setup();
        gameRunner->core->reset(gameRunner->core);
        _gameLoaded();
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
