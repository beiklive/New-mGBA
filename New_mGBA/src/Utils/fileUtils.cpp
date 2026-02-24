#include "Utils/fileUtils.hpp"

namespace beiklive::file
{
namespace fs = std::filesystem;

std::vector<std::string> listDir(std::string path)
{
    return listDir(path, SortBy::NameAsc);
}

std::vector<std::string> listDir(std::string path, SortBy sort_by)
{
    std::vector<std::string> entries;
    fs::path dir_path(path);

    // 检查路径是否存在且为目录
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path))
    {
        return entries; // 返回空容器
    }

    // 收集所有条目
    for (const auto& entry : fs::directory_iterator(dir_path))
    {
        entries.push_back(entry.path().string());
    }

    // 根据排序方式应用不同的排序规则
    switch (sort_by)
    {
        case SortBy::NameAsc:
            std::sort(entries.begin(), entries.end());
            break;

        case SortBy::NameDesc:
            std::sort(entries.begin(), entries.end(), std::greater<std::string>());
            break;

        case SortBy::TypeThenName:
        {
            // 目录在前，文件在后，各自按名称排序
            std::vector<std::string> dirs, files;
            for (const auto& entry : entries)
            {
                if (fs::is_directory(entry))
                {
                    dirs.push_back(entry);
                }
                else
                {
                    files.push_back(entry);
                }
            }
            std::sort(dirs.begin(), dirs.end());
            std::sort(files.begin(), files.end());

            entries.clear();
            entries.insert(entries.end(), dirs.begin(), dirs.end());
            entries.insert(entries.end(), files.begin(), files.end());
            break;
        }

        case SortBy::DirFirst:
        {
            // 目录在前，文件在后，保持各自原有的迭代器顺序
            std::vector<std::string> dirs, files;
            for (const auto& entry : entries)
            {
                if (fs::is_directory(entry))
                {
                    dirs.push_back(entry);
                }
                else
                {
                    files.push_back(entry);
                }
            }

            entries.clear();
            entries.insert(entries.end(), dirs.begin(), dirs.end());
            entries.insert(entries.end(), files.begin(), files.end());
            break;
        }

        case SortBy::FileFirst:
        {
            // 文件在前，目录在后，保持各自原有的迭代器顺序
            std::vector<std::string> dirs, files;
            for (const auto& entry : entries)
            {
                if (fs::is_directory(entry))
                {
                    dirs.push_back(entry);
                }
                else
                {
                    files.push_back(entry);
                }
            }

            entries.clear();
            entries.insert(entries.end(), files.begin(), files.end());
            entries.insert(entries.end(), dirs.begin(), dirs.end());
            break;
        }

        case SortBy::None:
        default:
            // 不排序，保持原顺序
            break;
    }

    return entries;
}

PathType getPathType(const std::string& path)
{
    fs::path p(path);
    if (!fs::exists(p))
        return PathType::NotExist;
    if (fs::is_directory(p))
        return PathType::Directory;
    return PathType::File;
}

std::string getParentPath(const std::string& path)
{
    std::filesystem::path p(path);
    std::filesystem::path parent = p.parent_path();
    return parent.string();
}


bool is_root_directory(const std::string& path_str) {
    if (path_str.empty()) return false;  // 空字符串肯定不是根目录

    try {
        std::filesystem::path p(path_str);
        // 规范化路径，移除多余的 '.'、'..' 和分隔符
        std::filesystem::path norm = p.lexically_normal();

        // 必须包含根目录组件，且相对路径部分为空
        return norm.has_root_directory() && norm.relative_path().empty();
    } catch (const std::exception&) {
        // 如果路径格式非法（例如含有无效字符），返回 false
        return false;
    }
}


} // namespace beiklive
