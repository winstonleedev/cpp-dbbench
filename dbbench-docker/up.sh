docker build -f Dockerfile-base -t thanhphu/dbbench-base .
docker build -f Dockerfile-src -t thanhphu/dbbench-src .
docker build -f Dockerfile-full -t thanhphu/dbbench .
docker push thanhphu/dbbench
docker run -p 2200:22 --restart always -v .data:/home/medium/medium/data -v .redis/redis.conf:/etc/redis/redis.conf thanhphu/dbbench