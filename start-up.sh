
#强制开启防火墙 端口
#sudo iptables -I INPUT -p tcp -m state --state NEW -m tcp --dport 22122 -j ACCEPT
#sudo iptables -I INPUT -p tcp -m state --state NEW -m tcp --dport 23000 -j ACCEPT
#sudo iptables -A INPUT -p tcp --dport 3306 -j ACCEPT

#启动本地tracker
#sudo /usr/bin/fdfs_trackerd ./conf/tracker.conf 
#启动本地storage
#sudo /usr/bin/fdfs_storaged ./conf/storage.conf

#spawn-fcgi -a 127.0.0.1 -p 8011 -f ./test/fcgi_test
spawn-fcgi -a 127.0.0.1 -p 8012 -f ./src/upload

#sudo /usr/local/nginx/sbin/nginx -s reload
