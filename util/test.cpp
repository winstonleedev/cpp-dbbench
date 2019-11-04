//
// Created by thanhphu on 19. 11. 4..
//

#include <util/test.h>

using namespace avis;

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

/**
 * Benchmarking test
 * @param opts test options
 */
void full_test(options opts) {
    std::cout << "Test for db type: " << opts.dbType << " initializing..." << std::endl << std::flush;
    auto dbPath = PATH_STATE_DB + std::to_string(opts.dbType);
    // Open database
    using namespace std::chrono_literals;
    StateDB *db = DBProvider::createSingle(dbPath, opts.dbType);
    if (!db->open(dbPath)) {
        std::cerr << "error: state db creation failure" << std::endl;
        exit(1);
    }

    std::string long_string(opts.stringLength - 3,  'x');

    while (!db->opened()) {
        std::this_thread::sleep_for(1s);
    }

    // Create some fixed value for reading
    for (int i = 0; i < 10000; i++) {
        db->put(std::to_string(i), long_string + std::to_string(randBig()));
    }

    // Counter start
    std::this_thread::sleep_for(1s);
    std::cout << "Initialization done. Test starting" << std::endl << std::flush;
    auto start = std::chrono::system_clock::now();
    auto end = start + std::chrono::seconds(opts.duration);

    long readCount = 0, writeCount = 0;
    auto* readResult = new std::string;
    while (std::chrono::system_clock::now() < end) {
        int index = rand10000();
        if (index < opts.readWeight) {
            readCount++;
            // Do read
            db->get(std::to_string(index), readResult);
        } else {
            writeCount++;
            // Do write
            db->put(std::to_string(index), long_string + std::to_string(randBig()));
        }
    }
    // Counter end, print results
    auto elapsed = std::chrono::system_clock::now() - start;
    auto elapsedSec = elapsed.count() / 1000000000.0;
    auto size = treeSize(dbPath);
    std::cout << "Test completed in " << elapsedSec << " s" << std::endl;
    std::cout << "Read  Ops: " << readCount << " (" << (readCount / elapsedSec) << " ops / sec)" << std::endl;
    std::cout << "Write Ops: " << writeCount << " (" << (writeCount / elapsedSec) << " ops / sec)" << std::endl;
    auto totalCount = readCount + writeCount;
    std::cout << "Total Ops: " << totalCount << " (" << (totalCount / elapsedSec) << " ops / sec)" << std::endl;
    std::cout << "Data size: " << size << std::endl;

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