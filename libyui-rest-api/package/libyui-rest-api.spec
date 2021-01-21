#
# spec file for package libyui-rest-api
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

Name:           libyui-rest-api

# DO NOT manually bump the version here; instead, use   rake version:bump
Version:        0.5.12
Release:        0

%define         so_version 14
%define         bin_name %{name}%{so_version}
%define         libyui_devel_version libyui-devel >= 3.10.1

BuildRequires:  cmake >= 3.10
BuildRequires:  gcc-c++
BuildRequires:  jsoncpp-devel
BuildRequires:  libmicrohttpd-devel
BuildRequires:  boost-devel
BuildRequires:  %{libyui_devel_version}

Summary:        Libyui - REST API plugin, the shared part
License:        LGPL-2.1-only OR LGPL-3.0-only
URL:            http://github.com/libyui/libyui-rest-api
Source:         %{name}-%{version}.tar.bz2

%description
This package provides a libyui REST API plugin.

It allows inspecting and controlling the UI remotely via
an HTTP REST API, it is designed for automated tests.

%package -n %{bin_name}
Summary:        Libyui - REST API plugin, the shared part
URL:            http://github.com/libyui/libyui-rest-api
Requires:       libyui%{so_version}
Requires:       yui_backend = %{so_version}
Provides:       %{name} = %{version}

%description -n %{bin_name}
This package provides a libyui REST API plugin.

It allows inspecting and controlling the UI remotely via
an HTTP REST API, it is designed for automated tests.


%package devel

Requires:       glibc-devel
Requires:       libstdc++-devel
Requires:       jsoncpp-devel
Requires:       libmicrohttpd-devel
Requires:       boost-devel
Requires:       %{libyui_devel_version}
Requires:       %{bin_name} = %{version}

URL:            http://github.com/libyui/
Summary:        Libyui header files

%description devel
This package provides a libyui REST API plugin.

This is a development subpackage.

%prep
%setup -q

%build

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
 -DDOC_DIR=%{_docdir} \
 -DLIB_DIR=%{_lib} \
 $CMAKE_OPTS

make %{?jobs:-j%jobs}


%install
cd build
make install DESTDIR="$RPM_BUILD_ROOT"
install -m0755 -d %{buildroot}/%{_libdir}/yui
install -m0755 -d %{buildroot}/%{_docdir}/%{bin_name}/
install -m0644 COPYING* %{buildroot}/%{_docdir}/%{bin_name}/


%post -n %{bin_name} -p /sbin/ldconfig
%postun -n %{bin_name} -p /sbin/ldconfig


%files -n %{bin_name}
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*
%doc %dir %{_docdir}/%{bin_name}
%license %{_docdir}/%{bin_name}/COPYING*


%files devel
%dir %{_docdir}/%{bin_name}
%{_libdir}/yui/lib*.so
%{_includedir}/yui

%changelog
