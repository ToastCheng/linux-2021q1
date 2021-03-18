#!/bin/bash
set -e

docker build . -t lab0
docker run \
    -v /Users/tushicheng/dev/jserv:/data \
    -it lab0 bash

