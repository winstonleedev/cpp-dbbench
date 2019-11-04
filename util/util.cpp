//
// Created by thanhphu on 19. 11. 4..
//

#include <util/util.h>

// Seed with a real random value, if available
std::random_device r;
std::default_random_engine random_engine(r());
std::uniform_int_distribution<int> uniform_dist(0, 100);
std::uniform_int_distribution<int> uniform_dist_big(0, 30000);

int rand100() {
    return (uniform_dist(random_engine));
}

int randBig() {
    return (uniform_dist_big(random_engine));
}

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
