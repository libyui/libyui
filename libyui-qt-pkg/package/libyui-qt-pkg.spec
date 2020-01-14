#
# spec file for package libyui-qt-pkg
#
# Copyright (c) 2019 SUSE LINUX GmbH, Nuernberg, Germany.
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


%define so_version 11
%define bin_name %{name}%{so_version}
%define libyui_qt_devel_version libyui-qt-devel >= 2.50.1
%define libzypp_devel_version libzypp-devel >= 17.21.0
Name:           libyui-qt-pkg
Version:        2.47.1
Release:        0
Summary:        Libyui - Qt Package Selector
License:        LGPL-2.1-only OR LGPL-3.0-only
URL:            https://github.com/libyui/
Source:         %{name}-%{version}.tar.bz2
BuildRequires:  %{libyui_qt_devel_version}
BuildRequires:  %{libzypp_devel_version}
BuildRequires:  cmake >= 2.8
BuildRequires:  gcc-c++
BuildRequires:  libqt5-qtbase-devel
BuildRequires:  libqt5-qtsvg-devel
BuildRequires:  libqt5-qtx11extras-devel
BuildRequires:  libyui-devel >= 3.9.0
BuildRequires:  pkgconfig
%if 0%{?suse_version} > 1325
BuildRequires:  libboost_headers-devel
%else
BuildRequires:  boost-devel
%endif

%description
This package contains the Qt package selector
component for libYUI.

%package -n %{bin_name}
Summary:        Libyui - Qt Package Selector
# bsc#1114654: Need Qt SVG support for icons (built-in and from theme)
Requires:       libQt5Svg5
Requires:       libyui%{so_version}
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

%description -n %{bin_name}
This package contains the Qt package selector
component for libYUI.

%package devel
Summary:        Libyui-qt-pkg header files
Requires:       %{bin_name} = %{version}
Requires:       %{libyui_qt_devel_version}
Requires:       %{libzypp_devel_version}

%description devel
This package contains the Qt package selector
component for libYUI.

This can be used independently of YaST for generic (C++) applications.
This package has very few dependencies.

%prep
%setup -q

%build
export CFLAGS="%{optflags} -DNDEBUG"
export CXXFLAGS="%{optflags} -DNDEBUG"

./bootstrap.sh %{_prefix}

%cmake \
  -DYPREFIX=%{_prefix} \
  -DDOC_DIR=%{_docdir} \
  -DLIB_DIR=%{_lib}
%cmake_build

%install
%cmake_install

%post -n %{bin_name} -p /sbin/ldconfig
%postun -n %{bin_name} -p /sbin/ldconfig

%files -n %{bin_name}
%license COPYING*
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*

%files devel
%{_libdir}/yui/lib*.so
%{_includedir}/yui
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/cmake/%{name}

%changelog
