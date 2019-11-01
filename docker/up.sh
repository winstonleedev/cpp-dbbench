docker pull thanhphu/dbbench
docker run --name dbbench -p 2200:22 --restart always -v "`pwd`/_data:/home/medium/medium/data" -v `pwd`/_redis/redis.conf:/etc/redis/redis.conf thanhphu/dbbench
