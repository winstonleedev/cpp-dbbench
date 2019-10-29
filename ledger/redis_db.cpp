#include <ledger/redis_db.h>

#include <iostream>

namespace avis {

RedisDB::~RedisDB() {
    close();
}

bool RedisDB::open(const std::string& dbname) {
    shouldRunScheduler = true;
    scheduler_thread = std::thread(
            [&]
            {
                while (shouldRunScheduler)
                {
                    ios.reset();
                    ios.run();
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            });

    con.connect(REDIS_IP,
                REDIS_PORT,
                [&] (boost::system::error_code const & error)
                {
                    std::cout << "* Connect callback: ";
                    if (error.value() != boost::system::errc::success)
                    {
                        std::cout << "failed. Keep trying..." << std::endl;
                    }
                    else
                    {
                        std::cout << "connected!" << std::endl;
                        isConnectionOpen = true;
                    }
                },
                [&] (boost::system::error_code const & ec)
                {
                    std::cout << "Connection lost. Error core: " << ec << ". Reconnecting..."<< std::endl;
                });
    return true;
}

void RedisDB::close() {
    isConnectionOpen = false;
    // Proper tear-down
    con.disconnect();
    con.sync_join();

    shouldRunScheduler = false;
    scheduler_thread.join();
}

bool RedisDB::opened() {
    return isConnectionOpen;
}

bool RedisDB::get(const std::string& key, std::string* value) {
    {
        std::unique_lock<std::mutex> lock(cv_mutex);
        con.execute([this, value](::nokia::net::proto::redis::reply &&reply) {
                        if (::nokia::net::proto::redis::reply::NIL == reply.type) {
                            return false;
                        } else {
                            if(value) *value = reply.str;
                        }
                        std::unique_lock<std::mutex> lock(cv_mutex);
                        cv.notify_one();
                    },
                    "GET", key);
        cv.wait_for(lock, timeout);
    }
    return true;
}

bool RedisDB::put(const std::string& key, const std::string& value) {
    {
        std::unique_lock<std::mutex> lock(cv_mutex);
        con.execute([this, value](::nokia::net::proto::redis::reply &&reply) {
                        std::unique_lock<std::mutex> lock(cv_mutex);
                        cv.notify_one();
                    },
                    "SET", key, value);
        cv.wait_for(lock, timeout);
    }
    return true;
}

bool RedisDB::putBatch(const std::string& key, const std::string& value) {
    put(key, value);
    return true;
}

bool RedisDB::del(const std::string& key) {
    {
        std::unique_lock<std::mutex> lock(cv_mutex);
        con.execute([this](::nokia::net::proto::redis::reply &&reply) {
                        std::unique_lock<std::mutex> lock(cv_mutex);
                        cv.notify_one();
                    },
                    "DEL", key);
        cv.wait_for(lock, timeout);
    }
    return true;
}

bool RedisDB::delBatch(const std::string& key) {
    del(key);
    return true;
}

bool RedisDB::applyBatch() {
    return true;
}

}
