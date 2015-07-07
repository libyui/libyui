![libYUI-boilerplate](http://img191.imageshack.us/img191/9364/libyui.png)

Libyui is a widget abstraction library providing Qt, GTK and ncurses
frontends. It was developed for [YaST](http://yast.github.io/).

### Building

Libyui uses CMake, driven by a slightly complex set of
[CMakefiles](https://github.com/libyui/libyui/tree/master/buildtools).

For reproducible builds it is best to use
[rubygem-packaging_rake_tasks.rpm](https://github.com/openSUSE/packaging_tasks)
like the [Jenkins CI](https://ci.opensuse.org/view/libyui/) jobs do.

You need [the base libyui library](https://github.com/libyui/libyui)
checked out in `../libyui` (or point the environment variable `LIBYUI_BASE`
to it).

Then run:
```
rake -f ../libyui/Rakefile osc:build
```
