#!/bin/bash
set -e

docker build -t gba-gravity-lab .

docker run --rm \
    -v "$(pwd)":/project \
    -w /project \
    gba-gravity-lab \
    make
