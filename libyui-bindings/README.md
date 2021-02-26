# LibYUI - The Language Bindings

[![Build Status](https://travis-ci.org/libyui/libyui-bindings.svg?branch=master
)](https://travis-ci.org/libyui/libyui-bindings)

[Libyui](https://github.com/libyui/libyui) is a widget abstraction library
providing Qt, GTK and ncurses frontends. Originally it was developed for
[YaST](https://yast.github.io/) but it can be used in any independent project.

The libyui library is written in C++, but this part contains bindings for other
programming languages like Perl, Ruby or Python.

### Ruby Notes

The API tries to follow these Ruby standards:

- Containers do not implement iterators but:
  - each
  - to_a
  - etc
- lower_case_methods instead of camelCaseMethods


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

