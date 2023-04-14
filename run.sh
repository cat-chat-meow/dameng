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

[ -n "$2" ] && echo "need build" && ./docker.sh -b demo$1 

./docker.sh -r demo$1 -name $run_name && \
    docker exec -it $run_name bash
