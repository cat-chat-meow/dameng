#!/bin/bash

print_usage(){
    echo "Usage: $0 [NUM]"
}

if [ $# -lt 1 ]; then
    print_usage
    exit 1
fi

run_name=""

if [ "$1" = "dmdb" ] 
then
    ./docker.sh -r dmdb -name dm8_01
    exit 0
fi

case $1 in
0) run_name="demo0" ;;
1) run_name="demo_cpp" ;;
2) run_name="demo2" ;;
3) run_name="demo_deps" ;;
*);;
esac

echo ">>> run"
echo "$1 $run_name"
echo ""

[ -n "$2" ] && echo "need build" && \
    ./docker.sh -b demo$1 && \
    ./docker.sh -r demo$1 -name $run_name


run_id=$(docker ps -a | grep "$run_name" | cut -d ' ' -f 1)
run_id_arr=(${run_id// /})
# 这个数组长度必为1 如果不为1 要看看 运行名字是否重叠 或者有其他问题
[ ${#run_id_arr[@]} -ne 1 ] && echo "!!! check your run name,  !!!"
if [ ${#run_id_arr[@]} -eq 0 ]; then
    # 没有在 run 的 立即 run
    ./docker.sh -r demo$1 -name $run_name    
    docker exec -it $run_name bash
    echo ""
else
    # 有的 过滤一下 看是 Up 还是 Exited
    run_info=$(docker ps -a | grep "$run_name")
    if [[ "$run_info" != *"Up"* ]]; then
        echo "is not running"
        # ./docker.sh -r demo$1 -name $run_name   
        docker start -it $run_name
        docker exec -it $run_name bash
    else
        echo "is running"
        docker exec -it $run_name bash
    fi
    echo ""
fi
