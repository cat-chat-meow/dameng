#!/bin/bash

# 遍历所有.h文件
for file in *.h; do
    echo "file name is: $file"
    # 获取文件当前编码格式
    encoding=$(file -b --mime-encoding "$file")
    # 判断是否为UTF-8编码格式
    if [ "$encoding" != "utf-8" ]; then
        # 转换文件编码格式为UTF-8
        iconv -f "$encoding" -t utf-8 -o "$file" "$file"
        echo "已将文件 $file 转换为UTF-8编码格式"
    fi
done

