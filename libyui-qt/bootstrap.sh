#!/bin/sh
set -e

prefix="/usr"

if [ ! -z $1 ]
then
  prefix="$1"
fi

cmake_common="$prefix/share/libyui/buildtools/CMakeLists.common"
cmake_target="./CMakeLists.txt"

echo "checking for $cmake_common..."

if [ -f "$cmake_common" ]
then
  ln -fs "$cmake_common" "$cmake_target"
  echo "OK: linked to `pwd`/$cmake_target."
else
  echo "  You must have libyui(-devel) >= 3.0.4 installed"
  echo "  in \"$prefix\" first !!!"
  exit 1
fi

exit 0
