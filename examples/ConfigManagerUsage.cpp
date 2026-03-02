#include <iostream>
#include <string>
#include <vector>
#include "Utils/ConfigManager.hpp"

int main() {
	using namespace beiklive;

	// 1) 构造：若文件存在会自动加载
	ConfigManager cfg("e:/MyCode/C++/New-mGBA/runtime/app.cfg");

	// 2) 写入：persist=false 的键仅在内存中，不会被 Save() 写入文件
	cfg.Set("width", ConfigValue(1280), true);
	cfg.Set("height", ConfigValue(720), true);
	cfg.Set("volume", ConfigValue(0.75f), true);
	cfg.Set("username", ConfigValue("alice"), true);

	cfg.Set("recent_files", ConfigValue(ConfigValue::StringArray{
		"rom_a.gba", "rom_b.gba"
	}), true);

	cfg.Set("hotkeys", ConfigValue(ConfigValue::IntArray{
		13, 27, 32
	}), true);

	cfg.Set("coeff", ConfigValue(ConfigValue::FloatArray{
		0.1f, 0.2f, 0.3f
	}), true);

	cfg.Set("session_token", ConfigValue("temp-token"), false); // 临时变量

	// 3) 保存到文件（仅保存 persist=true 的键）
	if (!cfg.Save()) {
		std::cerr << "save failed\n";
		return 1;
	}

	// 4) 重新读取验证
	ConfigManager cfg2("e:/MyCode/C++/New-mGBA/runtime/app.cfg");

	if (auto v = cfg2.Get("width"); v && v->AsInt()) {
		std::cout << "width=" << *v->AsInt() << "\n";
	}
	if (auto v = cfg2.Get("volume"); v && v->AsFloat()) {
		std::cout << "volume=" << *v->AsFloat() << "\n";
	}
	if (auto v = cfg2.Get("username"); v && v->AsString()) {
		std::cout << "username=" << *v->AsString() << "\n";
	}
	if (auto v = cfg2.Get("recent_files"); v && v->AsStringArray()) {
		std::cout << "recent_files size=" << v->AsStringArray()->size() << "\n";
	}

	// session_token 未落盘，重载后应不存在
	std::cout << "has session_token? " << (cfg2.Contains("session_token") ? "yes" : "no") << "\n";

	// 用例：
	const std::string oldPath = "E:/MyCode/python/mgbaListGen/name_map.cfg";
	const std::string newPath = "e:/MyCode/C++/New-mGBA/runtime/app.cfg";
	if (!beiklive::ConfigManager::ConvertLegacyFile(oldPath, newPath)) {
	    // oldPath 不存在或转换失败
        std::cerr << "Legacy file conversion failed or legacy file does not exist.\n";
	} else {
	    beiklive::ConfigManager cfg(newPath); // 自动读取
	    if (auto v = cfg.Get("mgba_dir_20260124_123108_1"); v && v->AsString()) {
	        // *v->AsString() == "节奏天国"
            std::cout << "mgba_dir_20260124_123108_1=" << *v->AsString() << "\n";
	    }
	}


	return 0;
}
