# Include Directory for libyui-rest-api based Plugins

This directory only consists of a subdirectory yui/ with a symlink

    rest-api -> ../../src

to enable consistent local builds of all libyui components (base lib and
plugins) without `make install` between changes:

Libyui-rest-api plugins include libyui-rest-api headers as

    #include <yui/rest-api/YFoo.h>

i.e. as public includes ("<...>") from a yui/rest-api subdirectory.

Normally, that would be /usr/include/yui/rest-api. But as we develop in the
libyui project, it is common to add some changes to libyui, then to libyui-qt,
then to libyui-qt-pkg; all those changes should be considered one transaction
to keep everything in a consistent state.

Having this include/yui subdirectory with its lone rest-api -> ../../src
symlink supports that: In the plugins' src/CMakefile.txt, add an include path

    target_include_directories( ${MYPLUGIN} BEFORE PUBLIC ../../libyui-rest-api/include )

to prefer the headers from this directory to the headers in /usr/include/yui/rest-api .

This is safe even if ../../libyui-rest-api/include is not available. In that
case, the system includes from /usr/include/yui/rest-api are used.
