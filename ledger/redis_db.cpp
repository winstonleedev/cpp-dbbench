#include <ledger/redis_db.h>

#include <iostream>

namespace avis {

RedisDB::~RedisDB() {
    close();
}

bool RedisDB::open(const std::string& dbname) {
    c = redisConnectUnixWithTimeout("/tmp/redis.sock", timeout);
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        return false;
    }
    isOpen = true;
    return true;
}

void RedisDB::close() {
    isOpen = false;
    redisFree(c);
}

bool RedisDB::opened() {
    return isOpen;
}

bool RedisDB::get(const std::string& key, std::string* value) {
    redisReply *reply = (redisReply *)redisCommand(c,"GET %s", key);
    bool result = true;
    if (reply->str == nullptr) {
        result = false;
    } else {
        *value = reply->str;
    }
    freeReplyObject(reply);
    return result;
}

bool RedisDB::put(const std::string& key, const std::string& value) {
    redisReply *reply = (redisReply *)redisCommand(c,"SET %s %s", key, value);
    bool result = isBatch || (strcmp(reply->str, "OK") == 0);
    freeReplyObject(reply);
    return result;
}

bool RedisDB::putBatch(const std::string& key, const std::string& value) {
    redisReply *reply = (redisReply *)redisCommand(c,"MULTI");
    isBatch = true;
    freeReplyObject(reply);
    return put(key, value);
}

bool RedisDB::del(const std::string& key) {
    redisReply *reply = (redisReply *)redisCommand(c,"DEL %s", key);
    bool result = isBatch || (reply->integer == 1);
    freeReplyObject(reply);
    return result;
}

bool RedisDB::delBatch(const std::string& key) {
    redisReply *reply = (redisReply *)redisCommand(c,"MULTI");
    isBatch = true;
    freeReplyObject(reply);
    return del(key);
}

bool RedisDB::applyBatch() {
    redisReply *reply = (redisReply *)redisCommand(c,"EXEC");
    freeReplyObject(reply);
    isBatch = false;
    return true;
}

}
