#include <ledger/level_db.h>

#include <iostream>
#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>

namespace avis {

LevelDB::~LevelDB() {

    close();
}

bool LevelDB::open(const std::string& dbname) {
    this->dbname = dbname;
    for (int idx = 0; idx < (1 << _shardBits); idx++) {
        leveldb::DB* ldb;
        leveldb::Options opts;
        leveldb::Status  status;

        opts.create_if_missing = true;
        opts.block_cache = leveldb::NewLRUCache((1 + 9 + 8 + 1 + 4) * 800 * 10000 / (1 << _shardBits) * 2);  // (1+9+8+1+4) * 800 * 10000 / 32 bytes * 2 = 36.8mb * 2
        //opts.block_cache       = leveldb::NewLRUCache(1024 * 1024 * 1024);
        //opts.filter_policy     = leveldb::NewBloomFilterPolicy(10);

        DB* db = new DB();
        db->db = ldb;
        db->opts = opts;
        db->status = leveldb::Status();
        db->batch = leveldb::WriteBatch();

        std::string path = dbname + "/" + std::to_string(idx);
        db->status = leveldb::DB::Open(db->opts, path, &db->db);
        if (!db->status.ok()) {
            #if __LOG_ERROR__
            std::cerr << "level db open error: " << status.ToString() << std::endl;
            #endif // __LOG_ERROR__
            return false;
        }

        _dbs.push_back(*db);
    }

    return true;
}

void LevelDB::close() {

    for (auto iter = _dbs.begin(); iter != _dbs.end(); iter++) {
        if (iter->db != nullptr) {
            delete iter->db;
        }

        if (iter->opts.block_cache != nullptr) {
            delete iter->opts.block_cache;
        }
    }
}

bool LevelDB::opened() {

    for (auto iter = _dbs.begin(); iter != _dbs.end(); iter++) {
        if (!iter->status.ok()) {
            return false;
        }
    }

    return true;
}

bool LevelDB::get(const std::string& key, std::string* value) {

    int idx = shard(hashSlice(key), _shardBits);
    _dbs[idx].status = _dbs[idx].db->Get(leveldb::ReadOptions(), key, value);

    if (!_dbs[idx].status.ok()) {
        return false;
    }

    return true;
}

bool LevelDB::put(const std::string& key, const std::string& value) {

    if (key.empty()) {
        return false;
    }

    int idx = shard(hashSlice(key), _shardBits);
    _dbs[idx].status = _dbs[idx].db->Put(leveldb::WriteOptions(), key, value);

    if (!_dbs[idx].status.ok()) {
        return false;
    }

    return true;
}

bool LevelDB::putBatch(const std::string& key, const std::string& value) {

    if (key.empty()) {
        return false;
    }

    int idx = shard(hashSlice(key), _shardBits);
    _dbs[idx].batch.Put(key, value);
    return true;
}

bool LevelDB::del(const std::string& key) {

    if (key.empty()) {
        return false;
    }

    int idx = shard(hashSlice(key), _shardBits);
    _dbs[idx].status = _dbs[idx].db->Delete(leveldb::WriteOptions(), key);

    if (!_dbs[idx].status.ok()) {
        return false;
    }

    return true;
}

bool LevelDB::delBatch(const std::string& key) {

    if (key.empty()) {
        return false;
    }

    int idx = shard(hashSlice(key), _shardBits);
    _dbs[idx].batch.Delete(key);
    return true;
}

bool LevelDB::applyBatch() {

    for (auto iter = _dbs.begin(); iter != _dbs.end(); iter++) {
        if (!(iter->db->Write(leveldb::WriteOptions(), &iter->batch)).ok()) {
            return false;
        }
    }

    for (auto iter = _dbs.begin(); iter != _dbs.end(); iter++) {
        iter->batch.Clear();
    }

    return true;
}

uint32_t LevelDB::shard(uint32_t hash, int shardBits) {

    return hash % (1 << shardBits);
}

uint32_t LevelDB::hashSlice(leveldb::Slice s) {

    return hash(s.data(), s.size(), 0);
}

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif

uint32_t LevelDB::hash(const char* data, size_t n, uint32_t seed) {

    // similar to murmur hash.
    const uint32_t m = 0xc6a4a793;
    const uint32_t r = 24;
    const char* limit = data + n;
    uint32_t       h = seed ^ (n * m);

    // pick up 4 bytes at a time.
    while (data + 4 <= limit) {
        uint32_t w = decodeFixed32(data);
        data += 4;
        h += w;
        h *= m;
        h ^= (h >> 16);
    }

    // pick up remaining bytes.
    switch (limit - data) {
    case 3:
        h += static_cast<unsigned char>(data[2]) << 16;
        FALLTHROUGH_INTENDED;

    case 2:
        h += static_cast<unsigned char>(data[1]) << 8;
        FALLTHROUGH_INTENDED;

    case 1:
        h += static_cast<unsigned char>(data[0]);
        h *= m;
        h ^= (h >> r);
        break;
    }

    return h;
}

uint32_t LevelDB::decodeFixed32(const char* ptr) {

    return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0]))) |
        (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8) |
        (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16) |
        (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
}

}
