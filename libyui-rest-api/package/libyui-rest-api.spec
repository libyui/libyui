#
# spec file for package libyui-rest-api
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


%define so_version 13
%define bin_name %{name}%{so_version}
%define libyui_devel_version libyui-devel >= 3.10.1

Name:           libyui-rest-api
Version:        0.5.5
Release:        0
Summary:        Libyui - REST API plugin, the shared part
License:        LGPL-2.1-only OR LGPL-3.0-only
Group:          System/Libraries
URL:            http://github.com/libyui/libyui-rest-api
Source:         %{name}-%{version}.tar.bz2

BuildRequires:  %{libyui_devel_version}
BuildRequires:  cmake >= 2.8
BuildRequires:  gcc-c++
BuildRequires:  jsoncpp-devel
BuildRequires:  libmicrohttpd-devel

%if 0%{?suse_version} > 1325
BuildRequires:  libboost_headers-devel
BuildRequires:  libboost_test-devel
%else
BuildRequires:  boost-devel
%endif

%description
This package provides a libyui REST API plugin.

It allows inspecting and controlling the UI remotely via
an HTTP REST API, it is designed for automated tests.

%package -n %{bin_name}
Summary:        Libyui - REST API plugin, the shared part
Group:          System/Libraries
URL:            http://github.com/libyui/libyui-rest-api
Requires:       libyui%{so_version}
Requires:       yui_backend = %{so_version}
Provides:       %{name} = %{version}

%description -n %{bin_name}
This package provides a libyui REST API plugin.

It allows inspecting and controlling the UI remotely via
an HTTP REST API, it is designed for automated tests.

%package devel
Summary:        Libyui header files
Group:          Development/Languages/C and C++
URL:            http://github.com/libyui/
Requires:       %{bin_name} = %{version}
Requires:       glibc-devel
Requires:       libstdc++-devel
Requires:       %{libyui_devel_version}
Requires:       jsoncpp-devel
Requires:       libmicrohttpd-devel
%if 0%{?suse_version} > 1325
Requires:       libboost_headers-devel
Requires:       libboost_test-devel
%else
Requires:       boost-devel
%endif

%description devel
This package provides a libyui REST API plugin.

This is a development subpackage.

%prep
%setup -q

%build

export CFLAGS="%{optflags} -DNDEBUG"
export CXXFLAGS="%{optflags} -DNDEBUG"

./bootstrap.sh %{_prefix}

# NOTE: %%cmake changes the CWD to "build" which is later expected by
# %%cmake_build, be careful when running additional commands later...
%cmake  -DYPREFIX=%{_prefix} \
        -DDOC_DIR=%{_docdir} \
        -DLIB_DIR=%{_lib} \
%if %{?_with_debug:1}%{!?_with_debug:0}
        -DCMAKE_BUILD_TYPE=RELWITHDEBINFO
%else
        -DCMAKE_BUILD_TYPE=RELEASE
%endif

%cmake_build

%install
%cmake_install
install -m0755 -d %{buildroot}/%{_docdir}/%{bin_name}/
install -m0755 -d %{buildroot}/%{_libdir}/yui
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
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/cmake/%{name}

%changelog
