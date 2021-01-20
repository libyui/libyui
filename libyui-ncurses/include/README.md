# Include Directory for libyui-ncurses based Plugins

This directory only consists of a subdirectory yui/ with a symlink

    ncurses -> ../../src

to enable consistent local builds of all libyui components (base lib and
plugins) without `make install` between changes:

Libyui-ncurses plugins include libyui-ncurses headers as

    #include <yui/ncurses/YQFoo.h>

i.e. as public includes ("<...>") from a yui/ncurses subdirectory.

Normally, that would be /usr/include/yui/ncurses. But as we develop in the
libyui project, it is common to add some changes to libyui, then to
libyui-ncurses, then to libyui-ncurses-pkg; all those changes should be
considered one transaction to keep everything in a consistent state.

Having this include/yui subdirectory with its lone ncurses -> ../../src symlink
supports that: In the plugins' src/CMakefile.txt, add an include path

    target_include_directories( ${MYPLUGIN} BEFORE PUBLIC ../../libyui-ncurses/include )

to prefer the headers from this directory to the headers in /usr/include/yui/ncurses .

This is safe even if ../../libyui-ncurses/include is not available. In that
case, the system includes from /usr/include/yui/ncurses are used.
