docker stop dbbench
docker rm dbbench
docker build -t dbbench .
docker tag dbbench thanhphu/dbbench
docker push thanhphu/dbbench
docker run --name dbbench -d thanhphu/dbbench