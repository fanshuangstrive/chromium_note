
# 测试环境

测试QUIC版本：Q043、Q039 
host配置 
  10.208.136.228  app.v.k.360kan.com
  y.play.360kan.com

Chrome浏览器测试上传文件地址：https://app.v.k.360kan.com/upload.html   
Chrome浏览器查看上传文件：https://app.v.k.360kan.com/upload/ 

# Chrome浏览器启用quic

1.启用quic： [chrome://flags/#enable-quic](chrome://flags/#enable-quic)

2.检查quic：

[chrome://net-internals/#quic](chrome://net-internals/#quic) 

安装 [http2-and-spdy-indicator](https://chrome.google.com/webstore/detail/http2-and-spdy-indicator/mpbpobfflnpcgagjijhmgnchggcjblin) 浏览器插件 

#  Quic 
参考 [QUIC Discovery](https://docs.google.com/document/d/1i4m7DbrWGgXafHxwl8SwIusY2ELUe8WX258xt2LFxPM/edit?pref=2&pli=1#heading=h.dk2fhev07ryt)

## GQUIC UDP 443

方式一、kquic  10.208.136.228 

```sh
kquic proxy start
```

方式二、chromium quic_server  10.173.125.17  要求：CentOS 7

```sh
# net/tools/quic/certs/
# --key_file pkcs8密钥
#  --v=1 打印详细日志
nohup ./quic_server --quic_proxy_backend_url=http://127.0.0.1:25080 --certificate_file=./y.play.360kan.com.crt --key_file=./y.play.360kan.com.pkcs8 --port=443 -mode=proxy &
```

```sh
openssl pkcs8 \
  -topk8 \
  -outform DER \
  -inform PEM \
  -in ./y.play.360kan.com.key \
  -out ./y.play.360kan.com.pkcs8 \
  -nocrypt
```

## nginx TCP 443 支持https ; 
```sh
cd /usr/local/openresty/nginx/
./sbin/nginx  -s reload

# cat nginx.conf | grep add_header
add_header Alt-Svc 'quic=":443"; ma=2592000; v="43,39"';
```

## php、php-fpm配置
```sh
/etc/php.ini 
 max_execution_time = 300
 post_max_size = 512M
 upload_max_filesize = 256M
/etc/php-fpm.conf
/etc/php-fpm.d/www.conf
php-fpm -R  --daemonize #Allow pool to run as root (disabled by default)

```

```sh
$netstat -anp | grep 443
tcp 0 0 0.0.0.0:443 0.0.0.0:* LISTEN 5599/nginx
tcp 0 0 :::443 :::* LISTEN 5599/nginx
udp 0 0 :::443 :::* 13653/kquic
$
```