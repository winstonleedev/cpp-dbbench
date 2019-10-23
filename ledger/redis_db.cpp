#include <ledger/redis_db.h>

#include <iostream>

namespace avis {

RedisDB::~RedisDB() {
    close();
}

bool RedisDB::open(const std::string& dbname) {
    std::condition_variable cv;
    std::mutex cv_mutex;

    ::nokia::net::redis_connection con(ios);

    {
        std::unique_lock<std::mutex> connect_lock(cv_mutex);
        con.connect("127.0.0.1",
                    6379,
                    [&] (boost::system::error_code const & error)
                    {
                        std::cout << "* Connect callback: ";
                        if (error)
                        {
                            std::cout << "failed. Keep trying..." << std::endl;
                        }
                        else
                        {
                            std::cout << "connected!" << std::endl;

                            std::unique_lock<std::mutex> lock(cv_mutex);
                            cv.notify_one();
                        }
                    },
                    [&] (boost::system::error_code const & ec)
                    {
                        std::cout << "Connection lost. Error core: " << ec << ". Reconnecting..."<< std::endl;
                    });


        cv.wait_for(connect_lock, std::chrono::seconds(5));
    }

    // Fail case

    std::cout << "* Looking for non-existing key...";
    {
        std::unique_lock<std::mutex> lock(cv_mutex);

        con.execute([&] (::nokia::net::proto::redis::reply && reply)
                    {

                        // std::cout << "Got reply: " << reply << std::endl;
                        if (::nokia::net::proto::redis::reply::NIL == reply.type)
                        {
                            std::cout << "not found. Good." << std::endl;
                        }
                        else
                        {
                            std::cout << "found. Might be an error or the key exists." << std::endl;
                        }
                        std::unique_lock<std::mutex> lock(cv_mutex);
                        cv.notify_one();
                    },
                    "GET", "I am pretty sure this key doesn't exist. #2018-11-20 16-51");

        cv.wait_for(lock, std::chrono::seconds(5));
    }


    // Success case

    std::cout << "* Fill DB...";
    {
        std::unique_lock<std::mutex> lock(cv_mutex);

        con.execute([&] (::nokia::net::proto::redis::reply && reply)
                    {

                        if (::nokia::net::proto::redis::reply::STRING == reply.type && "OK" == reply.str)
                        {
                            std::cout << "done." << std::endl;
                        }
                        else
                        {
                            std::cout << "something went wrong." << std::endl;
                            abort();
                        }
                        std::unique_lock<std::mutex> lock(cv_mutex);
                        cv.notify_one();
                    },
                    "SET", "this is a key", "...and this is a value");

        cv.wait_for(lock, std::chrono::seconds(5));
    }


    std::cout << "* Read DB...";
    {
        std::unique_lock<std::mutex> lock(cv_mutex);

        con.execute([&] (::nokia::net::proto::redis::reply && reply)
                    {

                        if (::nokia::net::proto::redis::reply::STRING == reply.type && "...and this is a value" == reply.str)
                        {
                            std::cout << "done." << std::endl;
                        }
                        else
                        {
                            std::cout << "something went wrong." << std::endl;
                            abort();
                        }
                        std::unique_lock<std::mutex> lock(cv_mutex);
                        cv.notify_one();
                    },
                    "GET", "this is a key");

        cv.wait_for(lock, std::chrono::seconds(5));
    }
    // Proper tear-down
    con.disconnect();
    con.sync_join();
    return true;
}

void RedisDB::close() {
    isConnectionOpen = false;
}

bool RedisDB::opened() {
    return isConnectionOpen;
}

bool RedisDB::get(const std::string& key, std::string* value) {
    return true;
}

bool RedisDB::put(const std::string& key, const std::string& value) {
    return true;
}

bool RedisDB::putBatch(const std::string& key, const std::string& value) {
    return true;
}

bool RedisDB::del(const std::string& key) {

    return true;
}

bool RedisDB::delBatch(const std::string& key) {
    return true;
}

bool RedisDB::applyBatch() {
    return true;
}

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif

uint32_t RedisDB::hash(const char* data, size_t n, uint32_t seed) {

    // similar to murmur hash.
    const uint32_t m = 0xc6a4a793;
    const uint32_t r = 24;
    const char* limit = data + n;
    uint32_t       h = seed ^ (n * m);

    // pick up 4 bytes at a time.
    while (data + 4 <= limit) {
        uint32_t w = decodeFixed32(data);
        data += 4;
        h += w;
        h *= m;
        h ^= (h >> 16);
    }

    // pick up remaining bytes.
    switch (limit - data) {
    case 3:
        h += static_cast<unsigned char>(data[2]) << 16;
        FALLTHROUGH_INTENDED;

    case 2:
        h += static_cast<unsigned char>(data[1]) << 8;
        FALLTHROUGH_INTENDED;

    case 1:
        h += static_cast<unsigned char>(data[0]);
        h *= m;
        h ^= (h >> r);
        break;
    }

    return h;
}

uint32_t RedisDB::decodeFixed32(const char* ptr) {

    return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0]))) |
        (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8) |
        (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16) |
        (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
}
}
