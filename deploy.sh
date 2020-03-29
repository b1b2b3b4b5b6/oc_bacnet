#!/usr/bin/sh

BIN_DIR="oc_bacnet"

tar -zvxf curl-7.63.0.tar.gz
cd curl-7.63.0
./configure
make all -j
make install -j 
echo '/usr/local/lib' >> /etc/ld.so.conf
ldconfig
cd ../
rm curl-7.63.0 -r
rm curl-7.63.0
make all -j
cd main
make all -j
sh rm_src.sh	
cd ../../
rm /usr/share/oc_bacnet -rf
mv oc_bacnet /usr/share
cd /etc/systemd/system
rm bacnet.service
ln /usr/share/oc_bacnet/service/bacnet.service
systemctl disable bacnet
systemctl enable bacnet
systemctl restart bacnet
