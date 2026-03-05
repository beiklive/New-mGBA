#include "Game/GameRuntime.hpp"
#include <chrono>
// 定义自动输入的特殊ID值
#define AUTO_INPUT 0x4E585031
// 音频采样缓冲大小（512个样本）
#define SAMPLES 0x200
// 音频缓冲区数量
#define N_BUFFERS 4
// 模拟摇杆的死区阈值（用于避免输入漂移）
#define ANALOG_DEADZONE 0x4000


static const GLfloat _offsets[] = {
	0.f, 0.f,
	1.f, 0.f,
	1.f, 1.f,
	0.f, 1.f,
};

static const GLchar* const _gles2Header =
	"#version 100\n"
	"precision mediump float;\n";

static const char* const _vertexShader =
	"attribute vec2 offset;\n"
	"uniform vec2 dims;\n"
	"uniform vec2 insize;\n"
	"varying vec2 texCoord;\n"

	"void main() {\n"
	"	vec2 ratio = insize;\n"
	"	vec2 scaledOffset = offset * dims;\n"
	"	gl_Position = vec4(scaledOffset.x * 2.0 - dims.x, scaledOffset.y * -2.0 + dims.y, 0.0, 1.0);\n"
	"	texCoord = offset * ratio;\n"
	"}";

static const char* const _fragmentShader =
	"varying vec2 texCoord;\n"
	"uniform sampler2D tex;\n"
	"uniform vec4 color;\n"

	"void main() {\n"
	"	vec4 texColor = vec4(texture2D(tex, texCoord).rgb, 1.0);\n"
	"	texColor *= color;\n"
	"	gl_FragColor = texColor;\n"
	"}";






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
static void _postAudioBuffer(struct mAVStream* stream, blip_t* left, blip_t* right) {
	UNUSED(stream);
	// _audioWait(0);
	// while (enqueuedBuffers >= N_BUFFERS - 1) {
	// 	if (!frameLimiter) {
	// 		blip_clear(left);
	// 		blip_clear(right);
	// 		return;
	// 	}
	// 	_audioWait(10000000);
	// }
	// if (enqueuedBuffers >= N_BUFFERS) {
	// 	blip_clear(left);
	// 	blip_clear(right);
	// 	return;
	// }
	// struct mStereoSample* samples = audioBuffer[audioBufferActive];
	// blip_read_samples(left, &samples[0].left, SAMPLES, true);
	// blip_read_samples(right, &samples[0].right, SAMPLES, true);
	// // audoutAppendAudioOutBuffer(&audoutBuffer[audioBufferActive]);
	// ++audioBufferActive;
	// audioBufferActive %= N_BUFFERS;
	// ++enqueuedBuffers;
}
static void _updateLux(struct GBALuminanceSource* lux) {
	UNUSED(lux);
}

uint8_t _readLux(struct GBALuminanceSource* lux) {
	if (!lux) {
		brls::Logger::warning("[GameRuntime] _readLux called with null lux");
		return static_cast<uint8_t>(0xFF - 0x16);
	}

	auto* runnerLux = reinterpret_cast<decltype(&gameRunner->luminanceSource)>(lux);
	int value = 0x16;
	if (runnerLux->luxLevel > 0) {
		value += GBA_LUX_LEVELS[runnerLux->luxLevel - 1];
	}
	return static_cast<uint8_t>(0xFF - value);
}
// 映射手柄按键到模拟器输入
static void _mapKey(struct mInputMap* map, uint32_t binding, int nativeKey, int key) {
	// 使用内置函数获取按位位置，绑定按键到输入映射
	mInputBindKey(map, binding, __builtin_ctz(nativeKey), key);
}

static void _updateLoading(size_t read, size_t size, void* context)
{
    static size_t lastPercent = static_cast<size_t>(-1);
    if (size == 0) {
        brls::Logger::warning("[GameRuntime] Loading progress unavailable: size=0");
        return;
    }

    size_t percent = (read * 100) / size;
    if (percent != lastPercent && (percent % 10 == 0 || percent == 100)) {
        brls::Logger::debug("[GameRuntime] Loading ROM: {}% ({} / {})", percent, read, size);
        lastPercent = percent;
    }
}

static void _setup()
{
	if (!gameRunner || !gameRunner->core) {
		brls::Logger::error("[GameRuntime] _setup aborted: core is null");
		return;
	}

	brls::Logger::debug("[GameRuntime] _setup begin");
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
	brls::Logger::debug("[GameRuntime] _setup done: AVStream set, audioBufferSize={}", SAMPLES);
}
static void _gameLoaded(){
    if (!gameRunner || !gameRunner->core) {
        brls::Logger::error("[GameRuntime] _gameLoaded aborted: core is null");
        return;
    }

    uint32_t samplerate = 115200;
	double ratio = GBAAudioCalculateRatio(1, 60.0, 1);
	blip_set_rates(gameRunner->core->getAudioChannel(gameRunner->core, 0), gameRunner->core->frequency(gameRunner->core), samplerate * ratio);
	blip_set_rates(gameRunner->core->getAudioChannel(gameRunner->core, 1), gameRunner->core->frequency(gameRunner->core), samplerate * ratio);

    brls::Logger::debug(
        "[GameRuntime] Audio configured: coreFreq={}, samplerate={}, ratio={}",
        gameRunner->core->frequency(gameRunner->core),
        samplerate,
        ratio
    );
}

namespace beiklive {
    


GameRuntime::GameRuntime(const std::string& gamePath)
{
    m_gamePath = gamePath;
    brls::Logger::info("[GameRuntime] Constructing runtime, path={}", m_gamePath);

    glInit();

	gameRunner->luminanceSource.d.readLuminance = _readLux;
	gameRunner->luminanceSource.d.sample = _updateLux;
	gameRunner->luminanceSource.luxLevel = 0;



	stream.videoDimensionsChanged = NULL;
	stream.postVideoFrame = NULL;
	stream.postAudioFrame = NULL;
	stream.postAudioBuffer = _postAudioBuffer;


}

GameRuntime::~GameRuntime()
{
    gameRunner->core->unloadROM(gameRunner->core);
    brls::Logger::info("[GameRuntime] Destroying runtime, path={}", m_gamePath);
    glDeinit();
}

void GameRuntime::loadGame()
{


    brls::Logger::info("[GameRuntime] loadGame begin: {}", m_gamePath);
    bool found = false;
    gameRunner->core = mCoreFind(m_gamePath.c_str());
    if (gameRunner->core) {
        brls::Logger::debug("[GameRuntime] Core found, initializing");
        gameRunner->core->init(gameRunner->core);
        mCoreInitConfig(gameRunner->core, gameRunner->port);
        mInputMapInit(&gameRunner->core->inputMap, &GBAInputInfo);

        struct VFile* rom = mDirectorySetOpenPath(&gameRunner->core->dirs, m_gamePath.c_str(), gameRunner->core->isROM);
        brls::Logger::debug("[GameRuntime] ROM open attempted: {}", rom ? "success" : "failed");

        found = mCorePreloadVFCB(gameRunner->core, rom, _updateLoading, gameRunner);


        if(!found) {
            if(rom) {
                rom->close(rom);
                brls::Logger::warning("[GameRuntime] ROM handle closed after preload failure");
            }
            gameRunner->core->deinit(gameRunner->core);
            brls::Logger::error("[GameRuntime] Failed to load ROM: {}", m_gamePath);
            return;
        }
        brls::Logger::info("[GameRuntime] ROM loaded successfully: {}", m_gamePath);

        if (gameRunner->core->platform(gameRunner->core) == mPLATFORM_GBA) {
		    gameRunner->core->setPeripheral(gameRunner->core, mPERIPH_GBA_LUMINANCE, &gameRunner->luminanceSource.d);
            gameRunner->gameFile.type = mPLATFORM_GBA;
            brls::Logger::debug("[GameRuntime] Platform detected: GBA");
        }else{
            gameRunner->gameFile.type = mPLATFORM_GB;
            brls::Logger::debug("[GameRuntime] Platform detected: GB");
        }

        gameRunner->core->setVideoGLTex(gameRunner->core, tex);
        brls::Logger::debug("[GameRuntime] GL texture registered to core: tex={}", tex);

        mCoreLoadForeignConfig(gameRunner->core, &gameRunner->config);
        mCoreAutoloadSave(gameRunner->core);
	    mCoreAutoloadCheats(gameRunner->core);
        brls::Logger::debug("[GameRuntime] Config/save/cheats autoload completed");

        _setup();
        gameRunner->core->reset(gameRunner->core);
        brls::Logger::debug("[GameRuntime] Core reset completed");

        _gameLoaded();

        m_gameLoaded = true;
        brls::Logger::info("[GameRuntime] loadGame end: success");
    } else {
        brls::Logger::error("[GameRuntime] Failed to initialize ROM core for game: {}", m_gamePath);
    }
}

void GameRuntime::glInit(void)
{
    brls::Logger::debug("[GameRuntime] glInit begin");
    int vwidth = 720, vheight = 480;
    // glViewport(0, 1080 - vheight, vwidth, vheight);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
    brls::Logger::debug("[GameRuntime] GL tex created: {}", tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &oldTex);
    brls::Logger::debug("[GameRuntime] GL oldTex created: {}", oldTex);
	glBindTexture(GL_TEXTURE_2D, oldTex);
	glTexParameteri(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenBuffers(1, &pbo);
    brls::Logger::debug("[GameRuntime] GL pbo created: {}", pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, 256 * 256 * 4, NULL, GL_STREAM_DRAW);
	// frameBuffer = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, 256 * 256 * 4, GL_MAP_WRITE_BIT);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glGenFramebuffers(1, &copyFbo);
    brls::Logger::debug("[GameRuntime] GL copyFbo created: {}", copyFbo);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, oldTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, copyFbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	program = glCreateProgram();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* shaderBuffer[2];

	shaderBuffer[0] = _gles2Header;

	shaderBuffer[1] = _vertexShader;
	glShaderSource(vertexShader, 2, shaderBuffer, NULL);

	shaderBuffer[1] = _fragmentShader;
	glShaderSource(fragmentShader, 2, shaderBuffer, NULL);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glCompileShader(fragmentShader);
	GLint success;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar msg[512];
		glGetShaderInfoLog(fragmentShader, sizeof(msg), NULL, msg);
		puts(msg);
        brls::Logger::error("[GameRuntime] Fragment shader compile failed: {}", msg);
	}

	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar msg[512];
		glGetShaderInfoLog(vertexShader, sizeof(msg), NULL, msg);
		puts(msg);
        brls::Logger::error("[GameRuntime] Vertex shader compile failed: {}", msg);
	}
	glLinkProgram(program);
    brls::Logger::debug("[GameRuntime] GL program linked: {}", program);

	texLocation = glGetUniformLocation(program, "tex");
	colorLocation = glGetUniformLocation(program, "color");
	dimsLocation = glGetUniformLocation(program, "dims");
	insizeLocation = glGetUniformLocation(program, "insize");
	GLuint offsetLocation = glGetAttribLocation(program, "offset");

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
    brls::Logger::debug("[GameRuntime] GL vbo/vao created: vbo={}, vao={}", vbo, vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_offsets), _offsets, GL_STATIC_DRAW);
	glVertexAttribPointer(offsetLocation, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(offsetLocation);
	glBindVertexArray(0);
    brls::Logger::debug("[GameRuntime] glInit end");
}
void GameRuntime::glDeinit(void) {
    brls::Logger::debug("[GameRuntime] glDeinit begin");
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glDeleteBuffers(1, &pbo);

	glDeleteFramebuffers(1, &copyFbo);
	glDeleteTextures(1, &tex);
	glDeleteTextures(1, &oldTex);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vao);
    brls::Logger::debug("[GameRuntime] glDeinit end");
}
void GameRuntime::_drawFrame()
{
	static uint64_t drawCount = 0;
    ++drawCount;

	unsigned width, height;
	gameRunner->core->desiredVideoDimensions(gameRunner->core, &width, &height);
    if ((drawCount % 10) == 0) {
        brls::Logger::debug("[GameRuntime] _drawFrame count={}, video={}x{}", drawCount, width, height);
    }

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glUseProgram(program);
	glBindVertexArray(vao);
	glUniform1i(texLocation, 0);
    glUniform2f(insizeLocation, 1, 1);
    glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_BLEND);

}

void GameRuntime::draw(NVGcontext* vg, float x, float y, float width, float height, brls::Style style, brls::FrameContext* ctx)
{
    static bool loggedNotLoaded = false;
    if (!m_gameLoaded) {
        if (!loggedNotLoaded) {
            brls::Logger::warning("[GameRuntime] draw skipped: game not loaded");
            loggedNotLoaded = true;
        }
        return;
    }

    using Clock = std::chrono::steady_clock;
    static auto lastFrameTime = Clock::now();
    constexpr auto frameInterval = std::chrono::microseconds(16667);

    auto now = Clock::now();
    if (now - lastFrameTime < frameInterval) {
        static uint64_t skipCount = 0;
        ++skipCount;
        if ((skipCount % 300) == 0) {
            brls::Logger::debug("[GameRuntime] draw throttled, skipCount={}", skipCount);
        }
        // return;
    }

    lastFrameTime = now;
    gameRunner->core->runFrame(gameRunner->core);
    _clearScreen();
    _drawFrame();
}

void GameRuntime::mInputMapInit(struct mInputMap* map, const struct mInputPlatformInfo* info) {
	map->maps = 0;
	map->numMaps = 0;
	map->info = info;
    brls::Logger::debug("[GameRuntime] InputMap initialized, nKeys={}", info ? info->nKeys : 0);
}

void GameRuntime::_clearScreen()
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}
};
