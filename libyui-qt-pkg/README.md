# LibYUI - The Qt Package Management Widget

[![Build Status](https://travis-ci.org/libyui/libyui-qt-pkg.svg?branch=master
)](https://travis-ci.org/libyui/libyui-qt-pkg)


Libyui is a widget abstraction library providing Qt, GTK and ncurses
frontends. Originally it was developed for [YaST](https://yast.github.io/)
but it can be used in any independent project.


This module provides the graphical (Qt) version of the YaST Package Selector
which is used to update, install or delete individual packages.

An overview about package handling in general is available on the
[libyui-ncurses-pkg wiki page](https://github.com/libyui/libyui-ncurses-pkg/wiki).
The wiki page also describes common use cases and how to solve the task in YaST
Software Management (for Qt as well as ncurses). Also included is the
information about related configuration files.

For details about reported bugs and issues see the wiki page
[background](https://github.com/libyui/libyui-ncurses-pkg/wiki/background).

### Building

Libyui uses CMake, driven by a slightly complex set of
[CMakefiles](https://github.com/libyui/libyui/tree/master/buildtools). For
reproducible builds it is best to use the [libyui-rake](
https://github.com/libyui/libyui-rake) Ruby gem like the [Jenkins CI](
https://ci.opensuse.org/view/libyui/) jobs do.

It can be installed from [rubygems.org](https://rubygems.org/gems/libyui-rake/)
using this command (Ruby needs to be installed in the system):

```
gem install libyui-rake
```

Then to build the package run:

```
rake osc:build
```
