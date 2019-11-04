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
    return true;
}

void RedisDB::close() {
}

bool RedisDB::opened() {
    return false;
}

bool RedisDB::get(const std::string& key, std::string* value) {
    redisReply *reply = (redisReply *)redisCommand(c,"GET %s", key);
    *value = reply->str;
    freeReplyObject(reply);
    return true;
}

bool RedisDB::put(const std::string& key, const std::string& value) {
    redisReply *reply = (redisReply *)redisCommand(c,"SET %s %s", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisDB::putBatch(const std::string& key, const std::string& value) {
    redisReply *reply = (redisReply *)redisCommand(c,"MULTI");
    freeReplyObject(reply);
    return put(key, value);
}

bool RedisDB::del(const std::string& key) {
    redisReply *reply = (redisReply *)redisCommand(c,"DEL %s", key);
    freeReplyObject(reply);
    return true;
}

bool RedisDB::delBatch(const std::string& key) {
    redisReply *reply = (redisReply *)redisCommand(c,"MULTI");
    freeReplyObject(reply);
    return del(key);
}

bool RedisDB::applyBatch() {
    redisReply *reply = (redisReply *)redisCommand(c,"EXEC");
    freeReplyObject(reply);
    return true;
}

}
