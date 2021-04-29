#
# spec file for package libyui-ncurses-pkg
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


Name:           libyui-ncurses-pkg

# DO NOT manually bump the version here; instead, use rake version:bump
Version:        4.2.8
Release:        0

%define         so_version 15
%define         libzypp_devel_version           libzypp-devel >= 17.21.0
%define         bin_name %{name}%{so_version}

BuildRequires:  %{libzypp_devel_version}
BuildRequires:  boost-devel
BuildRequires:  cmake >= 3.10
BuildRequires:  gcc-c++
BuildRequires:  libyui-devel >= %{version}
BuildRequires:  libyui-ncurses-devel >= %{version}
BuildRequires:  pkg-config

Summary:        Libyui - yast2 package selector widget for the NCurses UI
License:        LGPL-2.1-only OR LGPL-3.0-only
URL:            http://github.com/libyui/
Source:         libyui-%{version}.tar.bz2

%description
This package contains the package selector for the text based (NCurses) user
interface component for libyui.


%package -n %{bin_name}
Summary:        Libyui - yast2 package selector widget for the NCurses UI

Requires:       libyui%{so_version}
Requires:       libyui-ncurses%{so_version}
Provides:       %{name} = %{version}

Provides:       yast2-ncurses-pkg = 2.50.0
Obsoletes:      yast2-ncurses-pkg < 2.50.0

# Force removal of old -doc packages (bsc#1184363)
Obsoletes:      %{name}-doc < %{version}
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

Requires:       %{bin_name} = %{version}
Requires:       %{libzypp_devel_version}
Requires:       boost-devel
Requires:       glibc-devel
Requires:       libstdc++-devel
Requires:       libyui-ncurses-devel >= %{version}

%description devel

This package contains the header files for the NCurses (text based)
package selector component for libyui.


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
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
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

%changelog
