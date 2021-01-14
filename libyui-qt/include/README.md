# Include Directory for libyui-qt based Plugins

This directory only consists of a subdirectory yui/ with a symlink

    qt -> ../../src

to enable consistent local builds of all libyui components (base lib and
plugins) without `make install` between changes:

Libyui-qt plugins include libyui-qt headers as

    #include <yui/qt/YQFoo.h>

i.e. as public includes ("<...>") from a yui/qt subdirectory.

Normally, that would be /usr/include/yui/qt. But as we develop in the libyui
project, it is common to add some changes to libyui, then to libyui-qt, then to
libyui-qt-pkg; all those changes should be considered one transaction to keep
everything in a consistent state.

Having this include/yui subdirectory with its lone qt -> ../../src symlink
supports that: In the plugins' src/CMakefile.txt, add an include path

    target_include_directories( ${MYPLUGIN} BEFORE PUBLIC ../../libyui-qt/include )

to prefer the headers from this directory to the headers in /usr/include/yui/qt .

This is safe even if ../../libyui-qt/include is not available. In that case, the
system includes from /usr/include/yui/qt are used.
