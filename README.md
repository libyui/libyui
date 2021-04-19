# LibYUI - Widget Abstraction Library

[![Workflow Status](https://github.com/libyui/libyui/workflows/CI/badge.svg?branch=master)](https://github.com/libyui/libyui/actions?query=branch%3Amaster)
[![Jenkins Status](https://ci.opensuse.org/buildStatus/icon?job=libyui-master)](https://ci.opensuse.org/view/libyui/job/libyui-master/)


Libyui is a widget abstraction library providing graphical (Qt) and
text-based (NCurses) front-ends.

The is also a Gtk front-end with (limited) community support.

Originally, libyui was developed for [YaST](https://yast.github.io/), but it
can be also used in independent projects.


## End User's Perspective: Selecting the UI plug-in

By default, libyui tries to load any of the available UI plug-ins in this order:

- Qt:
  - if $DISPLAY is set
  - NCurses is user-selected and stdout is *not* a TTY

- Gtk:
  - if $DISPLAY is set and Qt is not available,
  - a GTK-based desktop environment is detected from the XDG_CURRENT_DESKTOP
    environment variable
  - any of the above pre-conditions are met and NCurses is user-selected, but
    stdout is *not* a TTY

- NCurses:
  - if $DISPLAY is *not* set and stdout is a TTY
  - Qt and Gtk are not available and stdout is a TTY


This default behaviour can be overridden by either:

- unsetting the DISPLAY environment variable to force NCurses:

      DISPLAY="" myprogram

- setting the environment variable YUI_PREFERED_BACKEND to one of
  - `gtk`
  - `ncurses`
  - `qt`

- specifing one of the switches on the command line of the program:
  - `--gtk`
  - `--ncurses`
  - `--qt`

  This overrides the environment variable.

If the user-selected UI plug-in is not installed on the system, an installed
UI plug-in will be chosen by the above criteria.


# Developer's and Packager's Perspective

## Components and Subprojects

This source repository contains the officially supported components as
subprojects:

- libyui: This is the base library that defines the API.

- libyui-qt: The Qt UI front-end
- libyui-qt-pkg: The package selector extension based on libzypp for the Qt UI
- libyui-qt-graph: A Qt UI extension for displaying Graphviz files

- libyui-ncurses: The NCurses (text-based) front-end
- libyui-ncurses-pkg: The package selector extension based on libzypp for the
  NCurses UI

- libyui-rest-api: A REST API for introspection and widgets remote control from
  a QA environment
- libyui-qt-rest-api: The Qt UI extension for the REST API
- libyui-ncurses-rest-api: The NCurses UI extension for the REST API

- libyui-bindings: SWIG bindings for Python, Perl, Ruby and Mono

  _Notice that YaST does not use this, it uses yast-ycp-ui-bindings based on
  YCPValue container classes._

There are also community-maintained components in separate repositories like

- libyui-gtk: The Gtk UI front-end
- libyui-mga: Extensions for Mageia Linux


# Building

Each subproject is self-sufficient (except for the shared VERSION.cmake file in
the toplevel directory) and can be built separately. They all use CMake, and
most operations are available from a very simple `Makefile.repo` file in the
subprojects directory. Of course you can also simply invoke _cmake_ manually.


## Building Manually in a Subproject

Using the Qt UI as an example:

    cd libyui-qt
    make -f Makefile.repo
    cd build
    make
    sudo make install

or

    cd libyui-qt
    make -f Makefile.repo build
    sudo make -C build install

Clean up with

    rm -rf build

or

    make -f Makefile.repo clean

Do all of this for the needed subprojects in order.

For YaST:

- libyui
- libyui-qt
- libyui-qt-pkg
- libyui-qt-graph
- libyui-ncurses
- libyui-ncurses-pkg

For non-YaST projects you may want to omit the -pkg and -graph parts, but build
the bindings:

- libyui
- libyui-qt
- libyui-ncurses
- libyui-bindings


## Building all Subprojects at Once

Use the `build-all` script in the project toplevel directory; it will build
each needed subproject in the correct sequence.

Call it with `-n` (`--dry-run`) to see what it _would_ do:

    ./build-all -n

    *** Dry run - not executing any make commands ***

    make -C libyui -f Makefile.repo build
    make -C libyui-qt -f Makefile.repo build
    make -C libyui-qt-graph -f Makefile.repo build
    make -C libyui-qt-pkg -f Makefile.repo build
    make -C libyui-ncurses -f Makefile.repo build
    make -C libyui-ncurses-pkg -f Makefile.repo build


For non-YaST projects, use `-s` (`--small`) for a small build
with the subprojects that are typically needed:

    ./build-all -n -s

    *** Dry run - not executing any make commands ***

    make -C libyui -f Makefile.repo build
    make -C libyui-qt -f Makefile.repo build
    make -C libyui-ncurses -f Makefile.repo build
    make -C libyui-bindings -f Makefile.repo build


or `-a` (`--all`) to build all subprojects:

    ./build-all -n -a

    *** Dry run - not executing any make commands ***

    make -C libyui -f Makefile.repo build
    make -C libyui-qt -f Makefile.repo build
    make -C libyui-qt-graph -f Makefile.repo build
    make -C libyui-qt-pkg -f Makefile.repo build
    make -C libyui-ncurses -f Makefile.repo build
    make -C libyui-ncurses-pkg -f Makefile.repo build
    make -C libyui-rest-api -f Makefile.repo build
    make -C libyui-qt-rest-api -f Makefile.repo build
    make -C libyui-ncurses-rest-api -f Makefile.repo build
    make -C libyui-bindings -f Makefile.repo build


or explicitly select subprojects to build with or without like
`-p` (`--no-pkg`) or `-g` (`--no-graph`) or `-r` (`--rest-api`).
See `./build-all -h` for an up-to-date complete list.


### Daily Development Work: Keeping an Existing Build

When working on libyui, it is common to change or add a class on the abstract
libyui level, then extend the concrete implementation in the Qt and NCurses
UIs; you don't want to rebuild everything from scratch after every little
change.

If you use `build-all build`, it will do just that: It will remove each
`build/` subdirectory in each subproject, recreate it, invoke `cmake`, then
`make`.

If you simply call it without any make target, however, it will check in each
subproject if there is an existing `build/` subdirectory, and if there is, just
call `make`. If there is no `build/` subdirectory, it will call `make -f
Makefile.repo build` instead. Notice that this handles each subproject
individually, so you can have a mixture of existing and non-existing `build/`
subdirectories.

    ./build-all

This builds everything from scratch.

Now continue editing files in subprojects and then

    ./build-all

This now just invokes `make` in each subproject, keeping the object
files for unchanged sources.

### make install

Of course you need root permissions to install any file to the system:

    ./build-all
    sudo ./build-all install


### Keeping Changes Locally in the Source Tree

The CMake environments in the subprojects are set up to prefer header files and
built libraries from sibling subprojects over those from the system; so you can
work for a long time in the source tree without a need for `sudo make install`.


# Version Numbers

All packages in this source tree have the same version numer; that's why the
subprojects share the toplevel `VERSION.cmake` file.

The general idea is to enable transaction-like changes on the libyui base
packages to avoid long delays in the distribution build cycle and broken builds:

Some parts may have become incompatible, yet the higher-level parts require the
base lib parts to be published as a prerequisite for building. In the past,
this resulted in staging projects not building; manual interaction was often
needed to break depencency cycles.

The version numbers are also in all the .spec files in the `package/`
subdirectory, so they need to be kept consistent among each other and with the
`VERSION.cmake` file.

For a simple version number increment, use

    rake version:bump

that does the required changes consistently.


This requires some more packages to be installed:

- rake (part of the ruby base package on SUSE distributions)
- the packaging_rake_tasks ruby gem
- the libyui-rake ruby gem

At the time of this writing, those packages were named

- ruby2.7
- ruby2.7-rubygem-libyui-rake
- ruby2.7-rubygem-packaging_rake_tasks

I.e. the package name of the gems contains the ruby version.
Use `zypper search` to find the current complete package name.


## Binary Compatibility and SO Version

Whenever there is an ABI change, the SO version needs to be bumped to the next
higher number, also in the toplevel `VERSION.cmake` and in all .spec files in
the `package/` subdirectory.

Use

    rake so_version:bump

(same package requirements as above)


### Binary Compatibility

The abstract libyui class uses the PIMPL idiom for API classes such as the
widgets: Each of the widget classes only has one single data member, the `priv`
pointer that holds a pointer to a private class holding all the real data
(e.g. YPushButtonPrivate). Adding data members to that private class is
perfectly safe and does not change the ABI (Application Binary Interface).

However, beware of any of the following (incomplete list):

- Data members are added to YUI or any similar class that is inherited in UI
  plug-ins

- Virtual functions are added or removed in API classes (or, again, in general
  in classes like YUI that are inherited in UI plug-ins)

- The order of virtual functions is changed (!) in any such class

- The inheritance hierarchy between API classes changes

- Enum values are added or removed

Exceptions:

- You can add a new virtual function at the end of a class.

- You can add a new enum value at the end of the enum.


If there is any doubt, better bump the SO version once too many rather than
once not enough. Making this easy and painless was one rationale behind the
changed libyui build environment and repo structure in early 2021, so please
use it.


## Building with Prefix

To install to another directory than `/usr`, set CMAKE_INSTALL_PREFIX;
either for each `cmake` call individually with `-D` or in the environment:

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
    make && make install

or

    CMAKE_INSTALL_PREFIX=/usr/local build-all -s

or

    export PREFIX=/usr/local
    build-all


# Building RPMs

For reproducible builds it is best to use the
[libyui-rake](https://github.com/libyui/libyui-rake)
Ruby gem like the [Jenkins CI](https://ci.opensuse.org/view/libyui/) jobs do.

It can be installed from [rubygems.org](https://rubygems.org/gems/libyui-rake/)
using this command (Ruby needs to be installed in the system):

```
gem install libyui-rake
```

Then to build the package run:

```
rake osc:build
```


### More info

Please visit the documentation at the
[doc folder](https://github.com/libyui/libyui/tree/master/libyui/doc)
for more information about
[how to branch](https://github.com/libyui/libyui/tree/master/libyui/doc/branching.md)
libyui and about
[auto-tagging](https://github.com/libyui/libyui/tree/master/libyui/doc/auto-tagging.md)
new versions.
