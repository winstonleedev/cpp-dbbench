/*
Copyright Medium Corp. 2019 All Rights Reserved.
creator : hs.kwon
*/

#include <ledger/db_provider.h>

#include <sys/types.h>
#include <sys/stat.h>

namespace avis {

DBProvider::DBProvider() {

}

DBProvider::~DBProvider() {

}

bool DBProvider::create(const std::string& dbname) {

    struct stat st;
    if (stat(dbname.c_str(), &st) != 0) {
        std::string cmd = "mkdir -p ";
        cmd.append(dbname);
        system(cmd.c_str());
    }

    _db = new LevelDB();
    if (!_db->open(dbname)) {
        return false;
    }

    return true;
}
}
