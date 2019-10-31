#include <ledger/lm_db.h>

#include <iostream>

namespace avis {

LMDB::~LMDB() {
    close();
}

bool LMDB::open(const std::string& dbname) {
    mdb_env_create(&env);
    int rc = mdb_env_open(env, dbname.c_str(), 0, 0664);;
    if (rc == 0) {
        isOpen = true;
    }
    return rc == 0;
}

void LMDB::close() {
    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}

bool LMDB::opened() {
    return isOpen;
}

bool LMDB::get(const std::string& key, std::string* value) {
    mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn);
    mdb_cursor_open(txn, dbi, &cursor);

    _key.mv_size = sizeof(char) * key.size();
    _key.mv_data = (void *)key.c_str();

    int rc = mdb_cursor_get(cursor, &_key, &_data, MDB_NEXT);
    if (rc == 0) {
        if(value) *value = std::string((char *) _data.mv_data);
    }
    mdb_cursor_close(cursor);
    mdb_txn_abort(txn);
    return rc == 0;
}

bool LMDB::put(const std::string& key, const std::string& value) {
    mdb_txn_begin(env, nullptr, 0, &txn);
    mdb_dbi_open(txn, nullptr, 0, &dbi);

    _key.mv_size = sizeof(char) * key.size();
    _key.mv_data = (void *)key.c_str();

    _data.mv_size = sizeof(char) * value.size();
    _data.mv_data = (void *)value.c_str();

    mdb_put(txn, dbi, &_key, &_data, MDB_NODUPDATA);
    return mdb_txn_commit(txn) == 0;
}

bool LMDB::putBatch(const std::string& key, const std::string& value) {
    if (!isBatch) {
        mdb_txn_begin(env, nullptr, 0, &txn);
        mdb_dbi_open(txn, nullptr, 0, &dbi);
        isBatch = true;
    }

    _key.mv_size = sizeof(char) * key.size();
    _key.mv_data = (void *)key.c_str();

    _data.mv_size = sizeof(char) * value.size();
    _data.mv_data = (void *)value.c_str();

    mdb_put(txn, dbi, &_key, &_data, MDB_NODUPDATA);
    return true;
}

bool LMDB::del(const std::string& key) {
    mdb_txn_begin(env, NULL, 0, &txn);

    _key.mv_size = sizeof(char) * key.size();
    _key.mv_data = (void *)key.c_str();

    int result = mdb_del(txn, dbi, &_key, nullptr);
    if (MDB_NOTFOUND == result) {
        mdb_txn_abort(txn);
    } else {
        mdb_txn_commit(txn);
    }
    return result != MDB_NOTFOUND;
}

bool LMDB::delBatch(const std::string& key) {
    del(key);
    return true;
}

bool LMDB::applyBatch() {
    bool result = mdb_txn_commit(txn) == 0;
    isBatch = false;
    return result;
}

}
