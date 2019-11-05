docker pull thanhphu/dbbench
docker stop dbbench
docker rm dbbench
docker run -d --name dbbench -p 2200:22 --restart always -v `pwd`/..:/src/dbbench -v `pwd`/_data:/home/medium/medium/data -v `pwd`/_redis/redis.conf:/etc/redis/redis.conf thanhphu/dbbench
docker logs -f dbbench