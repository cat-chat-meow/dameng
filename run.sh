#!/bin/bash

print_usage(){
    echo "Usage: $0 [NUM]"
}

if [ $# -lt 1 ]; then
    print_usage
    exit 1
fi

run_name=""

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

./docker.sh -b demo$1 && ./docker.sh -r demo$1 -name $run_name