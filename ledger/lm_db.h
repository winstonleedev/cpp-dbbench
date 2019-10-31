#pragma once

#include <string>
#include <vector>
#include <thread>

#include <boost/asio.hpp>

#include <lmdb/libraries/liblmdb/lmdb.h>

#include <ledger/state_db.h>

namespace avis {

class LMDB : public StateDB {

    friend class DBProvider;
    
public:
    ~LMDB() override;

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
    LMDB() = default;

    MDB_env *env;
    MDB_dbi dbi;
    MDB_val _key, _data;
    MDB_txn *txn;
    MDB_cursor *cursor, *cursor2;

    bool isOpen = false;
    bool isBatch = false;
};
}

