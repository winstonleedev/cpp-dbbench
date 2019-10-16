docker stop dbbench
docker rm dbbench
docker pull thanhphu/dbbench
docker build -t dbbench .
docker tag dbbench thanhphu/dbbench
docker push thanhphu/dbbench
docker run --name dbbench -d -p 2200:22 thanhphu/dbbench:latest