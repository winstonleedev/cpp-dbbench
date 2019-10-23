#pragma once

#include <string>
#include <vector>

#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/options.h>

#include <ledger/state_db.h>

namespace avis {

class RocksDB : public StateDB {

    friend class DBProvider;
    
public:
    ~RocksDB() override;

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
    struct DB {
        rocksdb::DB* db;
        rocksdb::Options    opts;
        rocksdb::Status     status;
        rocksdb::WriteBatch batch;
    };

    RocksDB() = default;

    uint32_t shard(uint32_t hash, int shardBits);
    uint32_t hashSlice(rocksdb::Slice s);
    uint32_t hash(const char* data, size_t n, uint32_t seed);
    uint32_t decodeFixed32(const char* ptr);

    std::vector<DB> _dbs;           // sharding dbs
    int             _shardBits = 4; // 4 bits (2 ^ 4 = 16 dbs)
};
}

