#pragma once

#include <string>
#include <vector>
#include <thread>

#include <boost/asio.hpp>

#include <lmdb/libraries/liblmdb/lmdb.h>

#include <ledger/state_db.h>

#define LM_DB_PATH "./test_lm_db"
#define E(expr) CHECK((rc = (expr)) == MDB_SUCCESS, #expr)
#define RES(err, expr) ((rc = expr) == (err) || (CHECK(!rc, #expr), 0))
#define CHECK(test, msg) ((test) ? (void)0 : ((void)fprintf(stderr, \
	"%s:%d: %s: %s\n", __FILE__, __LINE__, msg, mdb_strerror(rc)), abort()))

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
    MDB_cursor *cursor;

    bool isOpen = false;
    bool isBatch = false;
};
}

