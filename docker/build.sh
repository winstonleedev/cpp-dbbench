docker build -f Dockerfile-base -t thanhphu/dbbench-base .
docker build -f Dockerfile-src -t thanhphu/dbbench-src .
docker build -f Dockerfile-full -t thanhphu/dbbench .
docker push thanhphu/dbbench
