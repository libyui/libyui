#
# spec file for package libyui-qt-graph
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


Name:           libyui-qt-graph

# DO NOT manually bump the version here; instead, use   rake version:bump
Version:        4.2.16
Release:        0

%define         so_version 15
%define         bin_name %{name}%{so_version}

BuildRequires:  cmake >= 3.10
BuildRequires:  gcc-c++
BuildRequires:  graphviz-devel >= 2.26.0

# Used here and in src/CMakeLists.txt
BuildRequires:  libyui-devel >= %{version}
BuildRequires:  libyui-qt-devel >= %{version}
BuildRequires:  pkg-config
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5X11Extras)

Summary:        Libyui - Qt Graph Widget
License:        LGPL-2.1-only OR LGPL-3.0-only
URL:            http://github.com/libyui/
Source:         libyui-%{version}.tar.bz2

%description
This package contains the Qt graph component for libyui.


%package -n %{bin_name}
Summary:        Libyui - Qt graph widget
Requires:       libyui%{so_version}
Requires:       libyui-qt%{so_version}
Supplements:    libyui-qt%{so_version}
Provides:       %{name} = %{version}

Provides:       yast2-qt-graph = 2.46.0
Obsoletes:      yast2-qt-graph < 2.46.0
# Force removal of old -doc packages (bsc#1184363)
Obsoletes:      %{name}-doc < %{version}


%description -n %{bin_name}
This package contains the Qt graph component for libyui.

This is a special widget to visualize graphs such as the
storage device hierarchy (disks, partitions, subvolumes
etc.).  and similar graphviz-generated graphs.


%package devel
Summary:        Libyui - Header files for the Qt graph widget
Requires:       %{bin_name} = %{version}
Requires:       libyui-qt-devel >= %{version}

%description devel
This package contains the header files for the Qt graph component
for libyui.


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
 -DBUILD_EXAMPLES=on \
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
%{_includedir}/yui/qt-graph
%dir %{_docdir}/%{bin_name}
%doc %{_docdir}/%{bin_name}/examples

%changelog
