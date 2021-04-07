## Local installation

Here there is a step-by-step recipe about how to proceed when wanted to test *libyui* and related plugins local into home without the needs of admistration rights.

Cmake allows to customize [CMAKE_INSTALL_PREFIX](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html) and install the project in a different path.
That is not enough ofcourse since there is the need of finding and using the right libraries at run time and here the hint is setting **LD_LIBRARY_PATH** environment variable with the right directory where executable can search for the just built libyui shared library. Another usefull variable is **PKG_CONFIG_PATH** to help using [pkg-config](https://linux.die.net/man/1/pkg-config) and related cmake macros to find libraries.

Next script, just to simplify lib/lib64 choice, called _local.sh_ can be run from the building terminal as

`. local.sh `

so that environemnt variables are soon ready to be used. Let's assume to install all into $HOME/local, change **INST_DIR** for a different path (e.g. /usr/local for instance, recalling the need for system write access rights).

```
arch=`uname -m`
if [ "$arch" = "x86_64" ]; then
    lib="lib64"
else
    lib="lib"
fi

INST_DIR=$HOME/local
export LD_LIBRARY_PATH=${INST_DIR}/$lib/:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=${INST_DIR}/$lib/pkgconfig:$PKG_CONFIG_PATH
export PATH=${INST_DIR}/bin:${PATH}
```

After the above variables setup in the same terminal let's building libyui for debugging (remove _-DCMAKE_BUILD_TYPE=Debug_ otherwise). Form libyui root direcotry (the one with the _CMakeLists.txt_ file) run the following commands, similar to the target _configure_ into _Makefile.repo_:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$HOME/local ..
make install
```

Note that once run cmake, it is possible to use IDE like kdevelop for instance to change code and install it into the desired directory correctly.

To test plugins though building them also is necessary, so the above 4 lines should be performed for libyui-qt, libyui-ncurses, and so on and so for. To run executable depending on libyui, such as the examples for instance, the environemnt variables must be set too, so the terminal is still needed.
