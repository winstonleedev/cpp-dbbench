#pragma once

#include <string>
#include <vector>
#include <thread>

#include <boost/asio.hpp>

#include <wiredis/redis-connection.h>
#include <wiredis/tcp-connection.h>

#include <ledger/state_db.h>

namespace avis {

class RedisDB : public StateDB {

    friend class DBProvider;
    
public:
    ~RedisDB() override;

    bool open(const std::string& dbname) override;
    void close() override;
    bool opened() override;
    bool get(const std::string& key, std::string* value) override;
    bool put(const std::string& key, const std::string& value) override;
    bool putBatch(const std::string& key, const std::string& value) override;
    bool del(const std::string& key) override;
    bool delBatch(const std::string& key) override;
    bool applyBatch() override;

private:
    RedisDB() = default;

    ::boost::asio::io_service ios;
    ::nokia::net::redis_connection con = ::nokia::net::redis_connection(ios);
    bool isConnectionOpen = false;
    bool shouldRunScheduler = false;
    std::thread scheduler_thread;

    std::condition_variable cv;
    std::mutex cv_mutex;

    // Change this to change various timeouts in this class
    static constexpr auto timeout = std::chrono::seconds(5);
    static constexpr auto REDIS_IP = "localhost";
    static constexpr auto REDIS_PORT = 6379;

};
}

