#include <iostream>
#include <string>
#include "boost/program_options.hpp"

#include "ledger/db_provider.h"

#define DEFAULT_TX_COUNT 5000
#define _ENV(x, y)          (std::getenv(x) ? std::getenv(x) : y)
#define PATH_STATE_DB        _ENV("PATH_STATE_DB", "/home/medium/medium/data/states/")

using namespace avis;

namespace po = boost::program_options;

struct options {
    int dbType;
};

struct options handle_arguments(int ac, const char** av) {
    struct options result {};
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("db", po::value<int>(), "select database: 0 - level db, 1 - rocks db, 2 - redis")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        exit(0);
    }

    if (vm.count("db")) {
        result.dbType = vm["db"].as<int>();
    }
    return result;
}

int main(int argc, const char** argv) {
    auto options = handle_arguments(argc, argv);

    std::cout << "state db initialization start" << std::endl;

    if (!DBProvider::getInstance()->create(PATH_STATE_DB, options.dbType)) {
        std::cerr << "error: state db creation failure" << std::endl;
        return 1;
    }

    int to, from;
    char cto[9], cfrom[9];
    int ntx = DEFAULT_TX_COUNT;
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

