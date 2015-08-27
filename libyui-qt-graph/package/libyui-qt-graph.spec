#
# spec file for package libyui-qt-graph
#
# Copyright (c) 2014 SUSE LINUX Products GmbH, Nuernberg, Germany.
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
Version:        2.44.3
Release:        0
Source:         libyui-qt-graph-%{version}.tar.bz2

BuildRequires:  cmake >= 2.8
BuildRequires:  gcc-c++
BuildRequires:  graphviz-devel >= 2.26.0
BuildRequires:  libqt5-qtbase-devel
BuildRequires:  libqt5-qtsvg-devel
BuildRequires:  libqt5-qtx11extras-devel
BuildRequires:  libyui-devel >= 3.0.4

%define libyui_qt_devel_version libyui-qt-devel >= 2.42.0
BuildRequires:  %{libyui_qt_devel_version}

Url:            http://github.com/libyui/
Summary:        Libyui - Qt Graph Widget
License:        LGPL-2.1 or LGPL-3.0
Group:          System/Libraries

%description
This package contains the Qt graph 
component for libYUI.


%package -n libyui-qt-graph7

Requires:       libyui7
Supplements:    libyui-qt7
Provides:       libyui-qt-graph = %{version}

Provides:       yast2-qt-graph = 2.42.0
Obsoletes:      yast2-qt-graph < 2.42.0

Url:            http://github.com/libyui/
Summary:        Libyui - Qt Graph Widget
Group:          System/Libraries

%description -n libyui-qt-graph7
This package contains the Qt graph 
component for libYUI.



%package devel

Requires:       %{libyui_qt_devel_version}
Requires:       libyui-qt-graph7 = %{version}

Url:            http://github.com/libyui/
Summary:        Libyui-qt-graph header files
Group:          Development/Languages/C and C++

%description devel
This package contains the Qt graph 
component for libYUI.


This can be used independently of YaST for generic (C++) applications.
This package has very few dependencies.


%prep
%setup -q -n libyui-qt-graph-%{version}

%build

export CFLAGS="$RPM_OPT_FLAGS -DNDEBUG"
export CXXFLAGS="$RPM_OPT_FLAGS -DNDEBUG"

./bootstrap.sh %{_prefix}

mkdir build
cd build

%if %{?_with_debug:1}%{!?_with_debug:0}
cmake .. \
        -DYPREFIX=%{_prefix} \
        -DDOC_DIR=%{_docdir} \
        -DLIB_DIR=%{_lib} \
        -DCMAKE_BUILD_TYPE=RELWITHDEBINFO
%else
cmake .. \
        -DYPREFIX=%{_prefix} \
        -DDOC_DIR=%{_docdir} \
        -DLIB_DIR=%{_lib} \
        -DCMAKE_BUILD_TYPE=RELEASE
%endif

make %{?jobs:-j%jobs}

%install
cd build
make install DESTDIR="$RPM_BUILD_ROOT"
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/libyui-qt-graph7/
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0644 ../COPYING* $RPM_BUILD_ROOT/%{_docdir}/libyui-qt-graph7/

%clean
rm -rf "$RPM_BUILD_ROOT"

%post -n libyui-qt-graph7 -p /sbin/ldconfig

%postun -n libyui-qt-graph7 -p /sbin/ldconfig

%files -n libyui-qt-graph7
%defattr(-,root,root)
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*
%doc %dir %{_docdir}/libyui-qt-graph7
%doc %{_docdir}/libyui-qt-graph7/COPYING*

%files devel
%defattr(-,root,root)
%dir %{_docdir}/libyui-qt-graph7
%{_libdir}/yui/lib*.so
%{_prefix}/include/yui
%{_libdir}/pkgconfig/libyui-qt-graph.pc
%{_libdir}/cmake/libyui-qt-graph
%doc %{_docdir}/libyui-qt-graph7/examples

%changelog
