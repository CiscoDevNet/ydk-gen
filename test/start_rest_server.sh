#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
java -jar $DIR/moco-runner-0.11.0-standalone.jar http -p 12306 -c $DIR/db.json &> /dev/null  & moco_pid=$!

echo "$moco_pid"
