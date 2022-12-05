# Webserver (响应是hostname，如 http1 或 http2)
docker run -itd --rm --name=http1 --hostname=http1 feisky/webserver
docker run -itd --rm --name=http2 --hostname=http2 feisky/webserver

# # Client
# docker run -itd --rm --name=client alpine

# Nginx
docker run -itd --rm --name=nginx nginx


IP1=$(docker inspect http1 -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}')
IP2=$(docker inspect http2 -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}')
echo "Webserver1's IP: $IP1"
echo "Webserver2's IP: $IP2"




# 生成nginx.conf文件
cat>nginx.conf <<EOF
user  nginx;
worker_processes  auto;

error_log  /var/log/nginx/error.log notice;
pid        /var/run/nginx.pid;

events {
    worker_connections  1024;
}

http {
   include       /etc/nginx/mime.types;
   default_type  application/octet-stream;

    upstream webservers {
        server $IP1;
        server $IP2;
    }

    server {
        listen 80;

        location / {
            proxy_pass http://webservers;
        }
    }
}
EOF

# 更新Nginx配置
docker cp nginx.conf nginx:/etc/nginx/nginx.conf
docker exec nginx nginx -s reload





# # 查询Nginx容器IP（输出为172.17.0.5）
# docker inspect nginx -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}'

# 进入client容器终端，安装curl之后访问Nginx
# docker exec -it client sh

# # (以下命令运行在client容器中)
# / # apk add curl wrk --update
# / # curl "http://172.17.0.5"








# / # apk add wrk --update
# / # wrk -c100 "http://172.17.0.5"

IP_LB=$(docker inspect nginx -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}')
docker run -it --rm --name=client skandyla/wrk -c100 http://$IP_LB
