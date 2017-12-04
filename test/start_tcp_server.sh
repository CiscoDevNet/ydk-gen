#!/bin/bash

function start_server {
    DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    $DIR/tcp_proxy_server.py -b 12307 -c 2023 &> /dev/null &
    local status=$?
    if [ $status -ne 0 ]; then
        print_msg "Could not start tcp server"
        exit $status
    fi
}

start_server
tcp_pid=$!
echo "$tcp_pid"
