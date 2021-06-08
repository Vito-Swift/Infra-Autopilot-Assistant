#!/bin/bash

usage="$(basename "$0") [-h] [-b BUILD_MODE] [-c CLEAN]

Options:
  -h  show this help message
  -b  build program based on specified BUILD_MODE
      BUILD_MODE: ['lamp', 'hook']
  -c  clean build directories"

build() {
  echo "build on mode: $1"
  case "$1" in
  "lamp")
    mkdir build && cd build && cmake .. && make LamppostHost && cd ..
  ;;
  "hook")
  ;;
  *) printf "unknown build mode: %s\n" "$1" >&2
    echo "$usage" >&2
    exit 1
  ;;
  esac
}

clean() {
  echo "clean all temporary files"
  rm -r build
}

while getopts ':hb:c' option; do
  case "$option" in
    h) echo "$usage"
      exit
      ;;
    b) build $OPTARG
      exit
      ;;
    c) clean
      exit
      ;;
    :) printf "missing argument for -%s\n" "$OPTARG" >&2
      exit 1
      ;;
    \?) printf "illegal option: -%s\n" "$OPTARG" >&2
      echo "$usage" >&2
      exit 1
      ;;
  esac
done
shift $((OPTIND - 1))
