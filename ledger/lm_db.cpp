#include <ledger/lm_db.h>

#include <iostream>

namespace avis {

LMDB::~LMDB() {
    close();
}

bool LMDB::open(const std::string& dbname) {
    return true;
}

void LMDB::close() {
}

bool LMDB::opened() {
    return true;
}

bool LMDB::get(const std::string& key, std::string* value) {
    return true;
}

bool LMDB::put(const std::string& key, const std::string& value) {
    return true;
}

bool LMDB::putBatch(const std::string& key, const std::string& value) {
    put(key, value);
    return true;
}

bool LMDB::del(const std::string& key) {
    return true;
}

bool LMDB::delBatch(const std::string& key) {
    del(key);
    return true;
}

bool LMDB::applyBatch() {
    return true;
}

}
