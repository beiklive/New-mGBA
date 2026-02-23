#include "Utils/fileUtils.hpp"

namespace beiklive::file
{
    namespace fs = std::filesystem;

    std::vector<std::string> listDir(std::string path) {
        std::vector<std::string> entries;
        fs::path dir_path(path);

        // 检查路径是否存在且为目录
        if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
            return entries;  // 返回空容器
        }

        // 遍历目录
        for (const auto& entry : fs::directory_iterator(dir_path)) {
            // 获取文件名，转为普通字符串
            entries.push_back(entry.path().string());
        }

        return entries;
    }

    PathType getPathType(const std::string& path) {
        fs::path p(path);
        if (!fs::exists(p)) return PathType::NotExist;
        if (fs::is_directory(p)) return PathType::Directory;
        return PathType::File;
    }

} // namespace beiklive
