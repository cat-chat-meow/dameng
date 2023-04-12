# QA

mount -o loop dm8_20230104_x86_rh6_64.iso /mnt
mount: /mnt: mount failed: Unknown error -1

docker 启动没有使用 privileged=true

但是如果加上 privileged=true mysql 就启动不了了 真是绝活

- demo3 privileged 调试 达梦
- deps docker no privileged 使用 mysql 作为参考

## 调试通过的示例

- [odbc_conn](./odbctest/odbc_conn.c)
- [odbc_dml](./odbctest/odbc_dml.c)
- [odbc_bind](./odbctest/odbc_bind.c) 之前存在内存偏移，是因为类型不匹配导致 已修复

[odbc_lob](./odbctest/odbc_lob.c) 未调试
