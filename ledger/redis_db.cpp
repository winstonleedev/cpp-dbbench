#include <ledger/redis_db.h>

#include <iostream>

namespace avis {

RedisDB::~RedisDB() {
    close();
}

bool RedisDB::open(const std::string& dbname) {
    ConnectionOptions options;
    options.type = ConnectionType::UNIX;
    options.path = "/tmp/redis.sock";
    redis = new Redis(options);
    isOpen = true;
    return true;
}

void RedisDB::close() {
    delete redis;
    isOpen = false;
}

bool RedisDB::opened() {
    return isOpen;
}

bool RedisDB::get(const std::string& key, std::string* value) {
    auto result = redis->get(key);
    if (result) {
        *value = *result;
        return true;
    }
    return false;
}

bool RedisDB::put(const std::string& key, const std::string& value) {
    mtx.lock();
    bool result = redis->set(key, value);
    mtx.unlock();

    return result;
}

bool RedisDB::putBatch(const std::string& key, const std::string& value) {
    return put(key, value);
}

bool RedisDB::del(const std::string& key) {
    mtx.lock();
    bool result = redis->del(key);
    mtx.unlock();

    return result;
}

bool RedisDB::delBatch(const std::string& key) {
    return del(key);
}

bool RedisDB::applyBatch() {
    return true;
}

void RedisDB::clear() {
    redis->flushall(true);
}

}
