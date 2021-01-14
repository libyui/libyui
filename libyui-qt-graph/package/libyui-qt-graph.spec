#
# spec file for package libyui-qt-graph
#
# Copyright (c) 2014-2019 SUSE LINUX Products GmbH, Nuernberg, Germany.
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


Name:           libyui-qt-graph

# DO NOT manually bump the version here; instead, use   rake version:bump
Version:        2.46.6
Release:        0

%define         so_version 14
%define         libyui_devel_version libyui-devel >= 3.9.0
%define         libyui_qt_devel_version libyui-qt-devel >= 2.52.0
%define         bin_name %{name}%{so_version}

BuildRequires:  cmake >= 3.10
BuildRequires:  gcc-c++
BuildRequires:  graphviz-devel >= 2.26.0

# Used here and in src/CMakeLists.txt
BuildRequires:  pkg-config

BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5X11Extras)
BuildRequires:  pkgconfig(Qt5Svg)

BuildRequires:  %{libyui_qt_devel_version}
BuildRequires:  %{libyui_devel_version}

Summary:        Libyui - Qt Graph Widget
License:        LGPL-2.1 or LGPL-3.0
Url:            http://github.com/libyui/
Source:         %{name}-%{version}.tar.bz2

%description
This package contains the Qt graph component for libYUI.


%package -n %{bin_name}
Summary:        Libyui - Qt Graph Widget
Requires:       libyui%{so_version}
Supplements:    libyui-qt%{so_version}
Provides:       %{name} = %{version}

Provides:       yast2-qt-graph = 2.46.0
Obsoletes:      yast2-qt-graph < 2.46.0


%description -n %{bin_name}
This package contains the Qt graph component for libYUI.


%package devel
Summary:        Libyui-qt-graph header files
Requires:       %{bin_name} = %{version}
Requires:       %{libyui_qt_devel_version}

%description devel
This package contains the Qt graph component for libYUI.

This can be used independently of YaST for generic (C++) applications.
This package has very few dependencies.


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
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
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
%{_libdir}/yui/lib*.so
%{_includedir}/yui/qt-graph
%dir %{_docdir}/%{bin_name}
%doc %{_docdir}/%{bin_name}/examples

%changelog
