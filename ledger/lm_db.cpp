#include <ledger/lm_db.h>

#include <iostream>

namespace avis {

LMDB::~LMDB() {
    close();
}

bool LMDB::open(const std::string& dbname) {
    this->dbname = dbname;
    mdb_env_create(&env);
    int rc = mdb_env_open(env, dbname.c_str(), 0, 0664);;
    if (rc == 0) {
        isOpen = true;
    } else {
        return false;
    }
    mdb_txn_begin(env, nullptr, 0, &txn);
    mdb_dbi_open(txn, nullptr, 0, &dbi);
    return true;
}

void LMDB::close() {
    mdb_dbi_close(env, dbi);
    mdb_txn_commit(txn);
    mdb_env_close(env);
}

bool LMDB::opened() {
    return isOpen;
}

bool LMDB::get(const std::string& key, std::string* value) {
    _key.mv_size = sizeof(char) * key.size();
    _key.mv_data = (void *)key.c_str();

    int rc = mdb_get(txn, dbi, &_key, &_data);
    if (rc == 0) {
        if(value) *value = std::string((char *) _data.mv_data, _data.mv_size);
        return true;
    }
    return false;
}

bool LMDB::put(const std::string& key, const std::string& value) {
    _key.mv_size = sizeof(char) * key.size();
    _key.mv_data = (void *)key.c_str();

    _data.mv_size = sizeof(char) * value.size();
    _data.mv_data = (void *)value.c_str();

    return mdb_put(txn, dbi, &_key, &_data, MDB_NODUPDATA) == MDB_OK;
}

bool LMDB::putBatch(const std::string& key, const std::string& value) {
    _key.mv_size = sizeof(char) * key.size();
    _key.mv_data = (void *)key.c_str();

    _data.mv_size = sizeof(char) * value.size();
    _data.mv_data = (void *)value.c_str();

    return mdb_put(txn, dbi, &_key, &_data, MDB_NODUPDATA) == MDB_OK;
}

bool LMDB::del(const std::string& key) {
    _key.mv_size = sizeof(char) * key.size();
    _key.mv_data = (void *)key.c_str();

    return mdb_del(txn, dbi, &_key, nullptr) != MDB_NOTFOUND;
}

bool LMDB::delBatch(const std::string& key) {
    _key.mv_size = sizeof(char) * key.size();
    _key.mv_data = (void *)key.c_str();
    return mdb_del(txn, dbi, &_key, nullptr) != MDB_NOTFOUND;
}

bool LMDB::applyBatch() {
    return mdb_txn_commit(txn) == MDB_OK
        && mdb_txn_begin(env, nullptr, 0, &txn) == MDB_OK;
}

}
