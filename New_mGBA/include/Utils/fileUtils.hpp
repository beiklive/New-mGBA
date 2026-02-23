#pragma once
#include <string>
#include <vector>
#include <filesystem>

#include "Utils/strUtils.hpp"

// 对文件和目录的操作
namespace beiklive::file
{
    namespace fs = std::filesystem;
    enum class PathType { Directory, File, NotExist };
    enum class FileType { Normal, Image, Zip, GBA, GB};
/**
 * 获取指定路径下的所有文件和目录的名称（不含路径前缀）。
 * 若路径不存在或不是目录，返回空 vector。
 *
 * @param path 目标目录路径
 * @return 包含所有直接子项名称的 vector
 */
std::vector<std::string> listDir(std::string path);

/**
 * 判断路径是否存在，并区分是文件还是目录。
 */
PathType getPathType(const std::string& path);





} // namespace beiklive
