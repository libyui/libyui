#
# spec file for package libyui-qt
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


Name:           libyui-qt
Version:        2.46.18
Release:        0
Source:         libyui-qt-%{version}.tar.bz2

BuildRequires:  boost-devel
BuildRequires:  cmake >= 2.8
BuildRequires:  gcc-c++
BuildRequires:  pkg-config

%define libyui_devel_version libyui-devel >= 3.1.2
BuildRequires:  %{libyui_devel_version}
BuildRequires:  fontconfig-devel
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5X11Extras)
Provides:       yui_backend = 6

Url:            http://github.com/libyui/
Summary:        Libyui - Qt User Interface
License:        LGPL-2.1 or LGPL-3.0
Group:          System/Libraries

%description
This package contains the Qt user interface
component for libYUI.


%package -n libyui-qt6

Requires:       libyui6
Provides:       libyui-qt = %{version}
Provides:       yast2-qt = %{version}
Obsoletes:      yast2-qt < 2.42.0

Url:            http://github.com/libyui/
Summary:        Libyui - Qt User Interface
Group:          System/Libraries

%description -n libyui-qt6
This package contains the Qt user interface
component for libYUI.



%package devel

Requires:       %{libyui_devel_version}
Requires:       fontconfig-devel
Requires:       libyui-qt6 = %{version}

Url:            http://github.com/libyui/
Summary:        Libyui-qt header files
Group:          Development/Languages/C and C++

%description devel
This package contains the Qt user interface
component for libYUI.


This can be used independently of YaST for generic (C++) applications.
This package has very few dependencies.


%prep
%setup -q -n libyui-qt-%{version}

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
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/libyui-qt6/
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0644 ../COPYING* $RPM_BUILD_ROOT/%{_docdir}/libyui-qt6/

%clean
rm -rf "$RPM_BUILD_ROOT"

%post -n libyui-qt6 -p /sbin/ldconfig

%postun -n libyui-qt6 -p /sbin/ldconfig

%files -n libyui-qt6
%defattr(-,root,root)
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*
%doc %dir %{_docdir}/libyui-qt6
%doc %{_docdir}/libyui-qt6/COPYING*

%files devel
%defattr(-,root,root)
%dir %{_docdir}/libyui-qt6
%{_libdir}/yui/lib*.so
%{_prefix}/include/yui
%{_libdir}/pkgconfig/libyui-qt.pc
%{_libdir}/cmake/libyui-qt

%changelog
