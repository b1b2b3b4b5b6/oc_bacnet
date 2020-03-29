### oc sensor bancet服务器

### 必须clone到 /usr/share目录下

#### 编译：

进入 主目录

- make all

- cd main

- make all


#### 执行，建议使用root权限

./bacserv serverip:port

#### 自启（systemd）

服务文件：oc_bacnet/service/bacnet.service

