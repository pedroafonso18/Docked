#ifndef DOCKED_TEST_UTILS_H
#define DOCKED_TEST_UTILS_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace test_utils {

namespace fs = std::filesystem;

inline fs::path MakeTempDirectory(
    const std::string& prefix
)
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    fs::path path = fs::temp_directory_path() / (prefix + "-" + std::to_string(now));
    fs::create_directories(path);
    return path;
}

class ScopedCurrentPath
{
    public:
        explicit ScopedCurrentPath(
            const fs::path& path
        )
            : previousPath(fs::current_path())
        {
            fs::current_path(path);
        }

        ~ScopedCurrentPath()
        {
            fs::current_path(previousPath);
        }

        ScopedCurrentPath(const ScopedCurrentPath&) = delete;
        ScopedCurrentPath& operator=(const ScopedCurrentPath&) = delete;

    private:
        fs::path previousPath;
};

inline void WriteTextFile(
    const fs::path& path,
    const std::string& content
)
{
    fs::create_directories(path.parent_path());
    std::ofstream file(path);
    file << content;
}

inline void MakeExecutable(
    const fs::path& path
)
{
#ifndef _WIN32
    fs::permissions(
        path,
        fs::perms::owner_exec |
            fs::perms::group_exec |
            fs::perms::others_exec,
        fs::perm_options::add
    );
#else
    (void)path;
#endif
}

} // namespace test_utils

#endif