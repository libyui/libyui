#
# spec file for package libyui-qt-pkg
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


Name:           libyui-qt-pkg

# DO NOT manually bump the version here; instead, use rake version:bump
Version:        4.2.11
Release:        0

%define         so_version 15
%define         libzypp_devel_version libzypp-devel >= 17.21.0
%define         bin_name %{name}%{so_version}

BuildRequires:  boost-devel
BuildRequires:  cmake >= 3.10
BuildRequires:  gcc-c++
BuildRequires:  pkg-config

BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5X11Extras)

BuildRequires:  %{libzypp_devel_version}
BuildRequires:  libyui-devel >= %{version}
BuildRequires:  libyui-qt-devel >= %{version}

Summary:        Libyui - Qt Package Selector
License:        LGPL-2.1-only OR LGPL-3.0-only
URL:            https://github.com/libyui/
Source:         libyui-%{version}.tar.bz2

%description
This package contains the Qt package selector component for libyui.


%package -n %{bin_name}
Summary:        Libyui - Qt package selector

# bsc#1114654: Need Qt SVG support for icons (built-in and from theme)
Requires:       libQt5Svg5
# Selectable::hasRetracted()
Requires:       libzypp >= 17.21.0
# YPackageSelector#onlineSearchEnabled()
Requires:       libyui%{so_version} >= 3.9.2
Requires:       libyui-qt%{so_version}
Supplements:    (libyui-qt and yast2-packager)
Conflicts:      libqdialogsolver1 < 1.4.0
Provides:       %{name} = %{version}
Provides:       yast2-qt-pkg = 2.47.0
Obsoletes:      yast2-qt-pkg < 2.47.0
# force removal of all previous library versions (bsc#1148622),
# expands to: libyui-qt-pkg1 libyui-qt-pkg2 ... libyui-qt-pkg{so_version - 1}
Obsoletes:      %(echo `seq -s " " -f "libyui-qt-pkg%.f" $(expr %{so_version} - 1)`)
Provides:       libyui_pkg
# new packager is in qt5 and crashes if qt3 diagsolver is used (bnc#870683)
Obsoletes:      libqdialogsolver1 < 1.4.0
# Force removal of old -doc packages (bsc#1184363)
Obsoletes:      %{name}-doc < %{version}

%description -n %{bin_name}
This package contains the Qt package selector component for libyui.


%package devel
Summary:        Libyui-qt-pkg header files

Requires:       %{bin_name} = %{version}
Requires:       %{libzypp_devel_version}
Requires:       libyui-qt-devel >= %{version}

%description devel
This package contains the Qt package selector component for libyui.

This can be used independently of YaST for generic (C++) applications.
This package has very few dependencies.


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
%{_libdir}/yui/lib*.so
%{_includedir}/yui/qt-pkg

%changelog
