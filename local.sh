#!/bin/sh
# Copyright 2021 Angelo Naselli
# Copyright 2021 SUSE LLC
# MIT License
#
# The default build scripts assume you are using a container or a VM and
# get their dependencies from /usr and install into /usr.
# This script sets up a local user prefix on a production machine.

# Usage:
#   . local.sh
#   . local.sh ~/other-prefix
#
# then
#   cd libyui-foo; mkdir build; cd build
#   cmake_pfx ..
#   make && make install

INST_DIR=${1-$HOME/prefix-libyui}

# Run cmake using dependencies from INST_DIR and installing into INST_DIR
# Usage:
#   cmake_pfx ..
#   cmake_pfx -DCMAKE_BUILD_TYPE=Debug ..
cmake_pfx() {
    cmake \
        -DCMAKE_PREFIX_PATH="${INST_DIR}" \
        -DCMAKE_INSTALL_PREFIX="${INST_DIR}" \
        "$@"
}

if [ "$(uname -m)" = "x86_64" ]; then
    lib="lib64"
else
    lib="lib"
fi

export LD_LIBRARY_PATH=${INST_DIR}/$lib/:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=${INST_DIR}/$lib/pkgconfig:$PKG_CONFIG_PATH
export PATH=${INST_DIR}/bin:${PATH}

echo "PATH, LD_LIBRARY_PATH, PKG_CONFIG_PATH, cmake_pfx()"
echo "have been set up to use: $INST_DIR"
