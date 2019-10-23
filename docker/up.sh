docker stop dbbench
docker rm dbbench
docker build -t dbbench .
docker tag dbbench thanhphu/dbbench
# docker push thanhphu/dbbench
docker run --restart always --name dbbench -d -p 2200:22 thanhphu/dbbench:latest