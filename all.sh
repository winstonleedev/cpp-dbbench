./dbbench --db 0 --duration 60 > leveldb.txt
./dbbench --db 1 --duration 60 > rocksdb.txt
./dbbench --db 2 --duration 60 > redisdb.txt
./dbbench --db 3 --duration 60 > lmdb.txt