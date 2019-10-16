#include <iostream>
#include <string>

#include "ledger/db_provider.h"

#define DEFAULT_TXCOUNT 5000
#define _ENV(x, y)          (std::getenv(x) ? std::getenv(x) : y)
#define PATH_STATEDB        _ENV("PATH_STATEDB", "/home/medium/medium/data/states/")

using namespace avis;

int main(int argc, const char** argv) {

    std::cout << "state db initialization start" << std::endl;

    if (!DBProvider::getInstance()->create(PATH_STATEDB)) {
        std::cerr << "error: state db creation failure" << std::endl;
        return 1;
    }

    int to, from;
    char cto[9], cfrom[9];
    int ntx = DEFAULT_TXCOUNT;
    StateDB* db = DBProvider::getInstance()->get();
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < ntx; i++) {
            to   = (j + 1) * 1000000 + 1;
            from = (j + 1) * 10000000 + (j + 1) * 1000000 + 1;
            std::sprintf(cto, "%08X", i + to);
            std::sprintf(cfrom, "%08X", i + from);

            db->put(cto, "500");
            db->put(cfrom, "500");
        }
    }

    db->put("total-common-token", std::to_string(500 * 2 * ntx));
    std::cout << "total-common-token " << std::to_string(500 * 2 * ntx) << std::endl;
    std::cout << "state db initialization end" << std::endl;
    return 0;
}

