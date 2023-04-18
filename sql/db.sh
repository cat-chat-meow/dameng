#!/bin/bash

DB_DIRS=$(cd "$(dirname "$0")"; pwd)
MYSQL_USER=root
MYSQL_PASSWORD=123
DB_ARRAY=(test)

mysql -u$MYSQL_USER -p$MYSQL_PASSWORD mysql < $DB_DIRS/init_db.sql
for i in ${DB_ARRAY[@]};do
  echo "create database $i"
  mysql -u$MYSQL_USER -p$MYSQL_PASSWORD $i < $DB_DIRS/$i.sql
done

