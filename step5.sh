#!/bin/bash

. ./check_usr.sh

check_usr root

echo "registration service"
/dm8/script/root/dm_service_installer.sh -t dmserver -dm_ini /dm8/data/DAMENG/dm.ini -p DMSERVER
# finish create service dm then start
echo "systemctl start DmServiceDMSERVER.service"
systemctl start DmServiceDMSERVER.service