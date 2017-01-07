#!/bin/bash

java -jar moco-runner-0.11.0-standalone.jar http -p 12306 -c db.json &> /dev/null  & moco_pid=$!

echo "$moco_pid"
