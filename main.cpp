#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <random>

#include "boost/program_options.hpp"

#include "ledger/db_provider.h"

#define _ENV(x, y)          (std::getenv(x) ? std::getenv(x) : y)
#define PATH_STATE_DB        _ENV("PATH_STATE_DB", "/home/medium/medium/data/states-")

using namespace avis;

namespace po = boost::program_options;

// Data structures

struct options {
    int dbType;
    int readWeight;
    int writeWeight;
    int duration;
};

// Function declaration

void condition_test(bool expr, const std::string& message);

void integrity_test(int dbType);

void integrity_test();

void full_test(options opts);

// Parameters parsing

struct options handle_arguments(int ac, const char **av) {
    struct options result{};
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("db", po::value<int>(), "(required) select database: 0 - level db, 1 - rocks db, 2 - redis, 3 - LM DB")
            ("integrity", po::value<int>(), "perform integrity test for all databases")
            ("read", po::value<int>(), "ratio of read ops per 100 ops e.g 20, the rest will be write ops")
            ("duration", po::value<int>(), "how long should the test be run in seconds. default to 60");

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

    if (vm.count("read")) {
        result.readWeight = vm["read"].as<int>();
    } else {
        result.readWeight = 20;
    }
    result.writeWeight = 100 - result.readWeight;

    if (vm.count("duration")) {
        result.duration = vm["duration"].as<int>();
    } else {
        result.duration = 60;
    }
    return result;
}

// MAIN FUNCTION

int main(int argc, const char **argv) {
    auto options = handle_arguments(argc, argv);
    full_test(options);
    return 0;
}

/**
 * Test suite to see if libraries are working correctly (CRUD)
 */
void integrity_test() {
    std::cout << "Integrity test start" << std::endl;
    for (int dbType = 0; dbType < 4; dbType++) {
        std::cout << "== Testing DB Type " << dbType << " ==" << std::endl;
        integrity_test(dbType);
    }
    std::cout << "Integrity test end" << std::endl;
}

/**
 * Test the integrity of one type of database
 * @param dbType int: 0-3: type of database to test
 */
void integrity_test(int dbType) {
    std::cout << "Test for db type: " << dbType << " initializing..." << std::endl << std::flush;

    using namespace std::chrono_literals;
    StateDB *db = DBProvider::createSingle(PATH_STATE_DB + std::to_string(dbType), dbType);
    if (!db->open(PATH_STATE_DB + std::to_string(dbType))) {
        std::cerr << "error: state db creation failure" << std::endl;
        exit(1);
    }

    while (!db->opened()) {
        std::this_thread::sleep_for(1s);
    }

    std::cout << "Initialization done. Test starting\n" << std::flush;
    auto start = std::chrono::high_resolution_clock::now();

    condition_test(db->put("foo", "100"), "put single");
    condition_test(db->putBatch("bar", "200"), "put batch");
    condition_test(db->putBatch("baz", "300"), "put batch");
    condition_test(db->applyBatch(), "apply batch");

    std::this_thread::sleep_for(1s);

    auto *valueResult = new std::string();
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

    *valueResult = "";
    callResult = db->get("foo", valueResult);
    condition_test(!callResult, "deleted");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "Test completed in " << elapsed.count() << " ms\n";
    db->clear();
    delete db;
}

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

/**
 * Benchmarking test
 * @param opts test options
 */
void full_test(options opts) {
    std::cout << "Test for db type: " << opts.dbType << " initializing..." << std::endl << std::flush;

    // Open database
    using namespace std::chrono_literals;
    StateDB *db = DBProvider::createSingle(PATH_STATE_DB + std::to_string(opts.dbType), opts.dbType);
    if (!db->open(PATH_STATE_DB + std::to_string(opts.dbType))) {
        std::cerr << "error: state db creation failure" << std::endl;
        exit(1);
    }

    while (!db->opened()) {
        std::this_thread::sleep_for(1s);
    }

    // Create some fixed value for reading
    for (int i = 0; i < 100; i++) {
        db->put(std::to_string(i), std::to_string(randBig()));
    }

    // Counter start
    std::this_thread::sleep_for(1s);
    std::cout << "Initialization done. Test starting" << std::endl << std::flush;
    auto start = std::chrono::system_clock::now();
    auto end = start + std::chrono::seconds(opts.duration);

    long readCount = 0, writeCount = 0;
    auto* readResult = new std::string;
    while (std::chrono::system_clock::now() < end) {
        int index = rand100();
        if (index < opts.readWeight) {
            readCount++;
            // Do read
            db->get(std::to_string(index), readResult);
        } else {
            writeCount++;
            // Do write
            db->put(std::to_string(index), std::to_string(randBig()));
        }
    }
    // Counter end, print results
    auto elapsed = std::chrono::system_clock::now() - start;
    auto elapsedSec = elapsed.count() / 1000000000.0;
    std::cout << "Test completed in " << elapsedSec << " s" << std::endl;
    std::cout << "Read  Ops: " << readCount << " (" << (readCount / elapsedSec) << " ops / sec)" << std::endl;
    std::cout << "Write Ops: " << writeCount << " (" << (writeCount / elapsedSec) << " ops / sec)" << std::endl;
    auto totalCount = readCount + writeCount;
    std::cout << "Total Ops: " << totalCount << " (" << (totalCount / elapsedSec) << " ops / sec)" << std::endl;

    std::cout << opts.dbType << "," << elapsedSec << ","<< readCount << "," << std::endl;
    std::cout << opts.dbType << "," << elapsedSec << ","<< writeCount << "," << std::endl;

    // Clean up
    db->clear();
    delete db;
}

void condition_test(bool expr, const std::string& message) {
    if (expr) {
        std::cout << message << " passed" << std::endl;
    } else {
        std::cout << message << " failed" << std::endl;
    }
}
