#include <iostream>
#include <string>
#include <thread>

#include "boost/program_options.hpp"

#include <util/test.h>

namespace po = boost::program_options;
// Parameters parsing

struct options handle_arguments(int ac, const char **av) {
    struct options result{};
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("db", po::value<int>(), "(required) select database: 0 - level db, 1 - rocks db, 2 - redis, 3 - LM DB")
            ("integrity", "perform integrity test for all databases")
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

    result.stringLength = 5000;

    return result;
}

// MAIN FUNCTION

int main(int argc, const char **argv) {
    auto options = handle_arguments(argc, argv);
    full_test(options);
    return 0;
}

