#pragma once

#include <string>
#include <sys/stat.h>

namespace avis {

class StateDB {
    
public:
    virtual ~StateDB() = default;

    virtual bool open(const std::string& dbname) = 0;
    virtual void close() = 0;
    virtual bool opened() = 0;
    virtual bool get(const std::string& key, std::string* value) = 0;
    virtual bool put(const std::string& key, const std::string& value) = 0;
    virtual bool putBatch(const std::string& key, const std::string& value) = 0;
    virtual bool del(const std::string& key) = 0;
    virtual bool delBatch(const std::string& key) = 0;
    virtual bool applyBatch() = 0;

    virtual void clear() {
        struct stat st{};
        if (!dbname.empty() && stat(dbname.c_str(), &st) == 0) {
            std::string cmd = "rm -rf ";
            cmd.append(dbname);
            system(cmd.c_str());
        }
    };

protected:
    std::string dbname;
};
}
