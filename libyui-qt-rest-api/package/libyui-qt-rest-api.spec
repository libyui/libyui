#
# spec file for package libyui-qt-rest-api
#
# Copyright (c) 2019 SUSE LINUX GmbH, Nuernberg, Germany.
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

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#


Name:           libyui-qt-rest-api

# DO NOT manually bump the version here; instead, use   rake version:bump
Version:        4.0.0
Release:        0

%define         so_version 15
%define         libyui_devel_version libyui-devel >= 3.10.1
%define         bin_name %{name}%{so_version}

BuildRequires:  cmake >= 3.10
BuildRequires:  gcc-c++
BuildRequires:  boost-devel
BuildRequires:  libyui-qt-devel
BuildRequires:  libyui-rest-api-devel
BuildRequires:  %{libyui_devel_version}

BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5X11Extras)
BuildRequires:  pkgconfig(Qt5Svg)

Summary:        Libyui - The REST API plugin for the Qt frontend
License:        LGPL-2.1-only OR LGPL-3.0-only
URL:            http://github.com/libyui/
Source:         %{name}-%{version}.tar.bz2

%description
This package provides a libyui REST API plugin for the Qt frontend.

It allows inspecting and controlling the UI remotely via
an HTTP REST API. This is designed for automated tests.


%package -n %{bin_name}
Summary:        Libyui - The REST API plugin for the Qt frontend

Requires:       libyui%{so_version}
Requires:       libyui-rest-api%{so_version}
Requires:       yui_backend = %{so_version}
Provides:       %{name} = %{version}
Supplements:    (libyui-rest-api and libyui-qt)

%description -n %{bin_name}
This package provides a libyui REST API plugin for the Qt frontend.

It allows inspecting and controlling the UI remotely via
an HTTP REST API. This is designed for automated tests.


%package devel
Summary:        Libyui - Header files for the Qt REST API plugin

Requires:       %{bin_name} = %{version}
Requires:       glibc-devel
Requires:       libstdc++-devel
Requires:       libyui-qt-devel
Requires:       libyui-rest-api-devel


%description devel
This provides a libyui REST API plugin for the Qt frontend.

This package contains the header files for the plugin.

This is not needed for using the REST API, only for developing
extensions for it.


%prep
%setup -q

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
install -m0755 -d %{buildroot}/%{_libdir}/yui
install -m0755 -d %{buildroot}/%{_docdir}/%{bin_name}/
install -m0644 ../COPYING* %{buildroot}/%{_docdir}/%{bin_name}/


%post -n %{bin_name} -p /sbin/ldconfig
%postun -n %{bin_name} -p /sbin/ldconfig


%files -n %{bin_name}
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*
%doc %dir %{_docdir}/%{bin_name}
%license %{_docdir}/%{bin_name}/COPYING*


%files devel
%{_libdir}/yui/lib*.so
%{_includedir}/yui/qt-rest-api

%changelog
