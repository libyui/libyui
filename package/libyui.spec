#
# spec file for package libyui
#
# Copyright (c) 2021 SUSE LLC
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#


Name:           libyui

# DO NOT manually bump the version here; instead, use rake version:bump
Version:        4.2.10
Release:        0

%define         so_version 15
%define         bin_name %{name}%{so_version}

BuildRequires:  boost-devel
BuildRequires:  cmake >= 3.17
BuildRequires:  gcc-c++
BuildRequires:  libboost_test-devel
BuildRequires:  pkg-config

Summary:        GUI abstraction library
License:        LGPL-2.1-only OR LGPL-3.0-only
URL:            http://github.com/libyui/
Source:         %{name}-%{version}.tar.bz2

%description
This is the user interface engine that provides the abstraction from
graphical user interfaces (Qt, Gtk) and text based user interfaces
(ncurses).

Originally developed for YaST, it can also be used independently of
YaST for generic (C++) applications. This package has very few
dependencies.


%package -n %{bin_name}
Summary:        Libyui - GUI abstraction library

Provides:       yast2-libyui = 2.42.0
Obsoletes:      yast2-libyui < 2.42.0
Requires:       yui_backend = %{so_version}
# Force removal of old -doc packages (bsc#1184363)
Obsoletes:      %{name}-doc < %{version}

%description -n %{bin_name}
This is the user interface engine that provides the abstraction from
graphical user interfaces (Qt, Gtk) and text based user interfaces
(ncurses).

Originally developed for YaST, it can also be used independently of
YaST for generic (C++) applications. This package has very few
dependencies.


%package devel
Summary:        Libyui header files and examples

Requires:       %{bin_name} = %{version}
Requires:       boost-devel
Requires:       glibc-devel
Requires:       libstdc++-devel

%description devel

This package contains header files and examples for developing C++
applications based on libyui, the user interface engine that provides
the abstraction from graphical user interfaces (Qt, Gtk) and text
based user interfaces (ncurses).


%prep
%setup -q -n %{name}-%{version}

%build
pushd %{name}
mkdir build
cd build

export CFLAGS="$RPM_OPT_FLAGS -DNDEBUG $(getconf LFS_CFLAGS)"
export CXXFLAGS="$RPM_OPT_FLAGS -DNDEBUG $(getconf LFS_CFLAGS)"

%if %{?_with_debug:1}%{!?_with_debug:0}
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=RELWITHDEBINFO"
%else
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=RELEASE"
%endif

cmake .. \
 $CMAKE_OPTS

make %{?jobs:-j%jobs}
popd

%install
pushd %{name}
cd build
make install DESTDIR="$RPM_BUILD_ROOT"
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0644 ../../COPYING* $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
popd

%post -n %{bin_name} -p /sbin/ldconfig
%postun -n %{bin_name} -p /sbin/ldconfig

%files -n %{bin_name}
%defattr(-,root,root)
%{_libdir}/lib*.so.*
%doc %dir %{_docdir}/%{bin_name}
%license %{_docdir}/%{bin_name}/COPYING*

%files devel
%defattr(-,root,root)
%dir %{_docdir}/%{bin_name}
%{_libdir}/lib*.so
%{_includedir}/yui
%dir %{_datadir}/libyui
%{_datadir}/libyui/buildtools
%doc %{_docdir}/%{bin_name}/examples
%{_libdir}/pkgconfig/%{name}.pc
# %{_libdir}/cmake/%{name}

%changelog
