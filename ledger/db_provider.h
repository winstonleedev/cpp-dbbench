/*
Copyright Medium Corp. 2019 All Rights Reserved.
creator : hs.kwon
*/

#pragma once

#include <map>

#include <ledger/level_db.h>
#include <ledger/rocks_db.h>
#include <ledger/redis_db.h>

namespace avis {

class DBProvider {

public:
    virtual ~DBProvider();

    static DBProvider* getInstance() {
    
        static DBProvider* s_instance = new DBProvider();
        return s_instance;
    }
    
    bool create(const std::string& dbname, int dbType);
    StateDB* get() const { return _db; }

    enum DBType {
        LEVEL_DB = 0,
        ROCKS_DB = 1,
        REDIS = 2,
    };

private:
    DBProvider();

    StateDB* _db;
};
}
