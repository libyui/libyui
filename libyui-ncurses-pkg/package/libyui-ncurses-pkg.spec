#
# spec file for package libyui-ncurses-pkg
#
# Copyright (c) 2015-2019 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (c) 2020-2021 SUSE LLC, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#


Name:           libyui-ncurses-pkg

# DO NOT manually bump the version here; instead, use   rake version:bump
Version:        2.50.10
Release:        0

%define         so_version 14
%define         libyui_devel_version libyui-devel >= 3.10.0
%define         libyui_ncurses_devel_version    libyui-ncurses-devel >= 2.54.0
%define         libzypp_devel_version           libzypp-devel >= 17.21.0
%define         bin_name %{name}%{so_version}


BuildRequires:  cmake >= 3.10
BuildRequires:  gcc-c++
BuildRequires:  boost-devel
BuildRequires:  libyui-devel >= 3.9.0
BuildRequires:  pkg-config

BuildRequires:  %{libyui_devel_version}
BuildRequires:  %{libyui_ncurses_devel_version}
BuildRequires:  %{libzypp_devel_version}

Summary:        Libyui - yast2 package selector widget for the NCurses UI
License:        LGPL-2.1-only OR LGPL-3.0-only
Url:            http://github.com/libyui/
Source:         %{name}-%{version}.tar.bz2

%description
This package contains the package selector for the text based (NCurses) user
interface component for libyui.


%package -n %{bin_name}
Summary:        Libyui - yast2 package selector widget for the NCurses UI

Requires:       libyui%{so_version}
Provides:       %{name} = %{version}

Provides:       yast2-ncurses-pkg = 2.50.0
Obsoletes:      yast2-ncurses-pkg < 2.50.0

# force removal of all previous library versions (bsc#1148622),
# expands to: libyui-ncurses-pkg1 libyui-ncurses-pkg2 ... libyui-ncurses-pkg{so_version - 1}
Obsoletes:      %(echo `seq -s " " -f "libyui-ncurses-pkg%.f" $(expr %{so_version} - 1)`)

Provides:       libyui_pkg
Supplements:    packageand(libyui-ncurses:yast2-packager)

# Selectable::hasRetracted()
Requires:       libzypp >= 17.21.0


%description -n %{bin_name}
This package contains the NCurses (text based) package selector
component for libyui.


%package devel
Summary:        Libyui-ncurses-pkg header files

Requires:       %{libyui_ncurses_devel_version}
Requires:       %{libzypp_devel_version}
Requires:       boost-devel
Requires:       %{bin_name} = %{version}
Requires:       glibc-devel
Requires:       libstdc++-devel


%description devel

This package contains the header files for the NCurses (text based)
package selector component for libyui.


%prep
%setup -q -n %{name}-%{version}

%build

export CFLAGS="$RPM_OPT_FLAGS -DNDEBUG"
export CXXFLAGS="$RPM_OPT_FLAGS -DNDEBUG"

mkdir build
cd build

%if %{?_with_debug:1}%{!?_with_debug:0}
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=RELWITHDEBINFO"
%else
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=RELEASE"
%endif

cmake .. \
 -DDOC_DIR=%{_docdir} \
 -DLIB_DIR=%{_lib} \
 $CMAKE_OPTS

make %{?jobs:-j%jobs}


%install
cd build
make install DESTDIR="$RPM_BUILD_ROOT"
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0644 ../COPYING* $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/


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

%changelog
