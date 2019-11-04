#pragma once

#include <string>
#include <vector>
#include <thread>

#include <boost/asio.hpp>

#include <hiredis/hiredis.h>

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

    redisContext *c;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
};
}

