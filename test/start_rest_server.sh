#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [[ ! -e $DIR/moco-runner-0.11.0-standalone.jar ]]
then
    cd $DIR
    wget https://repo1.maven.org/maven2/com/github/dreamhead/moco-runner/0.11.0/moco-runner-0.11.0-standalone.jar
    cd - &> /dev/null
fi
java -jar $DIR/moco-runner-0.11.0-standalone.jar http -p 12306 -c $DIR/db.json > /dev/null  & moco_pid=$!

echo "$moco_pid"
