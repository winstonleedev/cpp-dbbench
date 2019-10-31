#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "boost/program_options.hpp"

#include "ledger/db_provider.h"

#define DEFAULT_TX_COUNT 5000
#define _ENV(x, y)          (std::getenv(x) ? std::getenv(x) : y)
#define PATH_STATE_DB        _ENV("PATH_STATE_DB", "/home/medium/medium/data/states-")

using namespace avis;

namespace po = boost::program_options;

struct options {
    int dbType;
};

void condition_test(bool expr, std::string message);
void integrity_test(int dbType);
void integrity_test();

struct options handle_arguments(int ac, const char** av) {
    struct options result {};
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("db", po::value<int>(), "select database: 0 - level db, 1 - rocks db, 2 - redis")
            ("integrity", "perform integrity test for all databases")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        exit(0);
    }

    if (vm.count("integrity")) {
        integrity_test();
        exit(0);
    }

    if (vm.count("db")) {
        result.dbType = vm["db"].as<int>();
    } else {
        exit(0);
    }
    return result;
}

int main(int argc, const char** argv) {
    auto options = handle_arguments(argc, argv);

    return 0;

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

void integrity_test() {
    std::cout << "Integrity test start" << std::endl;
    for (int dbType = 0; dbType < 4; dbType++) {
        std::cout << "== Testing DB Type " << dbType << " ==" << std::endl;
        integrity_test(dbType);
    }
    std::cout << "Integrity test end" << std::endl;

}

void integrity_test(int dbType) {
    using namespace std::chrono_literals;
    StateDB* db = DBProvider::createSingle(PATH_STATE_DB + std::to_string(dbType), dbType);

    if (!db->open(PATH_STATE_DB + std::to_string(dbType))) {
        std::cerr << "error: state db creation failure" << std::endl;
        exit(1);
    }

    while (!db->opened()) {
        std::this_thread::sleep_for(1s);
    }

    condition_test(db->put("foo", "100"), "put single");
    condition_test(db->putBatch("bar", "200"), "put batch");
    condition_test(db->putBatch("baz", "300"), "put batch");
    condition_test(db->applyBatch(), "apply batch");

    std::this_thread::sleep_for(1s);

    auto* valueResult = new std::string();
    auto callResult = db->get("foo", valueResult);
    condition_test(callResult, "call get");
    condition_test(*valueResult == "100", "get result match");

    callResult = db->get("bar", valueResult);
    condition_test(callResult, "");
    condition_test(*valueResult == "200", "");

    condition_test(db->del("foo"), "del single");
    condition_test(db->delBatch("bar"), "del batch");
    condition_test(db->delBatch("baz"), "");
    condition_test(db->applyBatch(), "");

    std::this_thread::sleep_for(1s);

    callResult = db->get("foo", valueResult);
    condition_test(!callResult, "deleted");

    delete db;
}

void condition_test(bool expr, std::string message) {
    if (expr) {
        std::cout << message << " passed" << std::endl;
    } else {
        std::cout << message << " failed" << std::endl;
    }
}
