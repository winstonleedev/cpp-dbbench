//
// Created by thanhphu on 19. 11. 4..
//

#include <util/util.h>

long treeSize(const fs::path& pathToShow)
{
    auto size = 0;
    if (fs::exists(pathToShow) && fs::is_directory(pathToShow))
    {
        for (const auto& entry : fs::directory_iterator(pathToShow))
        {
            auto filename = entry.path().filename();
            if (fs::is_directory(entry.status()))
            {
                size += treeSize(entry);
            }
            else if (fs::is_regular_file(entry.status())) {
                size += fs::file_size(entry.path());
            }
        }
    }
    return size;
}
