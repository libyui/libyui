#
# spec file for package libyui-ncurses
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


Name:           libyui-ncurses

# DO NOT manually bump the version here; instead, use rake version:bump
Version:        4.2.11
Release:        0

%define         so_version 15
%define         bin_name %{name}%{so_version}

BuildRequires:  boost-devel
BuildRequires:  cmake >= 3.10
BuildRequires:  gcc-c++
BuildRequires:  libyui-devel >= %{version}
BuildRequires:  ncurses-devel

URL:            http://github.com/libyui/
Summary:        Libyui - NCurses (text based) user interface
License:        LGPL-2.1-only OR LGPL-3.0-only
Source:         libyui-%{version}.tar.bz2

%description
This package contains the NCurses (text based) user interface
component for libyui.


%package -n %{bin_name}
Summary:        Libyui - NCurses (text based) user interface

Requires:       glibc-locale
Requires:       libyui%{so_version}
Provides:       %{name} = %{version}
Provides:       yast2-ncurses = 2.42.0
Obsoletes:      yast2-ncurses < 2.42.0
Provides:       yui_backend = %{so_version}
# Force removal of old -doc packages (bsc#1184363)
Obsoletes:      %{name}-doc < %{version}

%description -n %{bin_name}
This package contains the NCurses (text based) user interface
component for libyui.

%package devel
Summary:        Libyui - Header fles for the NCurses (text based) user interface

Requires:       %{bin_name} = %{version}
Requires:       boost-devel
Requires:       glibc-devel
Requires:       libstdc++-devel
Requires:       libyui-devel >= %{version}
Requires:       ncurses-devel

%description devel
This package contains the header files for the NCurses
(text based) user interface component for libyui.

This package is not needed to develop libyui-based applications,
only to develop extensions for libyui-ncurses.


%package tools

Summary:        Libyui - tools for the NCurses (text based) user interface
Requires:       screen
# conflict with libyui-ncurses8, /usr/bin/libyui-terminal was originally there
Conflicts:      %{name}8

%description tools
This package contains tools for the NCurses (text based)
user interface component for libyui:

libyui-terminal - useful for testing on headless machines


%prep
%setup -q -n libyui-%{version}

%build
export CFLAGS="$RPM_OPT_FLAGS -DNDEBUG"
export CXXFLAGS="$RPM_OPT_FLAGS -DNDEBUG"

pushd %{name}
mkdir build
cd build

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
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
install -m0644 ../../COPYING* $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
popd

%post -n %{bin_name} -p /sbin/ldconfig
%postun -n %{bin_name} -p /sbin/ldconfig

%files -n %{bin_name}
%defattr(-,root,root)
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*
%doc %dir %{_docdir}/%{bin_name}
%license %{_docdir}/%{bin_name}/COPYING*

%files devel
%defattr(-,root,root)
%dir %{_docdir}/%{bin_name}
%{_libdir}/yui/lib*.so
%{_prefix}/include/yui
%{_libdir}/pkgconfig/%{name}.pc

%files tools
%defattr(-,root,root)
%{_bindir}/libyui-terminal

%changelog
