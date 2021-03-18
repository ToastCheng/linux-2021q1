#!/bin/bash
set -e

docker build . -t lab0
docker run \
    -v /Users/tushicheng/dev/linux-2021q1:/data \
    -it lab0 bash

