# LibYUI - The Qt Package Dependency Extension

[![Build Status](https://travis-ci.org/libyui/libyui-qt-graph.svg?branch=master
)](https://travis-ci.org/libyui/libyui-qt-graph)


[Libyui](https://github.com/libyui/libyui) is a widget abstraction library
providing Qt, GTK and ncurses frontends. Originally it was developed for
[YaST](https://yast.github.io/) but it can be used in any independent project.

This part contains an extension for the Qt graphical frontend which displays
the package dependencies as a graph.

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

