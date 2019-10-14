/*
Copyright Medium Corp. 2019 All Rights Reserved.
creator : hs.kwon
*/

#pragma once

#include <map>

#include "ledger/level_db.h"

namespace avis {

class DBProvider {

public:
    virtual ~DBProvider();

    static DBProvider* getInstance() {
    
        static DBProvider* s_instance = new DBProvider();
        return s_instance;
    }
    
    bool create(const std::string& dbname);
    StateDB* get() const { return _db; }

private:
    DBProvider();

    StateDB* _db;
};
}
