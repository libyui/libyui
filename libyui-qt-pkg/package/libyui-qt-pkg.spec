#
# spec file for package libyui-qt-pkg
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


Name:           libyui-qt-pkg
Version:        2.45.22
Release:        0
Source:         %{name}-%{version}.tar.bz2

%define so_version 9
%define bin_name %{name}%{so_version}

%if 0%{?suse_version} > 1325
BuildRequires:  libboost_headers-devel
%else
BuildRequires:  boost-devel
%endif
BuildRequires:  cmake >= 2.8
BuildRequires:  gcc-c++
BuildRequires:  libqt5-qtbase-devel
BuildRequires:  libqt5-qtsvg-devel
BuildRequires:  libqt5-qtx11extras-devel
BuildRequires:  libyui-devel >= 3.0.4
BuildRequires:  pkg-config

%define libyui_qt_devel_version libyui-qt-devel >= 2.43.3
BuildRequires:  %{libyui_qt_devel_version}
%define libzypp_devel_version           libzypp-devel >= 15.14.0
BuildRequires:  %{libzypp_devel_version}

Url:            http://github.com/libyui/
Summary:        Libyui - Qt Package Selector
License:        LGPL-2.1 or LGPL-3.0
Group:          System/Libraries

%description
This package contains the Qt package selector
component for libYUI.


%package -n %{bin_name}

Requires:       libyui%{so_version}
Provides:       %{name} = %{version}

Provides:       yast2-qt-pkg = 2.42.0
Obsoletes:      yast2-qt-pkg < 2.42.0

Provides:       libyui_pkg
Supplements:    packageand(libyui-qt:yast2-packager)
# control center gnome requires graphical pkg and gtk is dropped (bnc#999031)
Supplements:    packageand(yast2-control-center-gnome:yast2-packager)

# new packager is in qt5 and crashes if qt3 diagsolver is used (bnc#870683)
Obsoletes:      libqdialogsolver1 < 1.4.0
Conflicts:      libqdialogsolver1 < 1.4.0

Url:            http://github.com/libyui/
Summary:        Libyui - Qt Package Selector
Group:          System/Libraries

%description -n %{bin_name}
This package contains the Qt package selector
component for libYUI.



%package devel

Requires:       %{libyui_qt_devel_version}
Requires:       %{libzypp_devel_version}
Requires:       %{bin_name} = %{version}

Url:            http://github.com/libyui/
Summary:        Libyui-qt-pkg header files
Group:          Development/Languages/C and C++

%description devel
This package contains the Qt package selector
component for libYUI.


This can be used independently of YaST for generic (C++) applications.
This package has very few dependencies.


%prep
%setup -q -n %{name}-%{version}

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
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0644 ../COPYING* $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/

%clean
rm -rf "$RPM_BUILD_ROOT"

%post -n %{bin_name} -p /sbin/ldconfig

%postun -n %{bin_name} -p /sbin/ldconfig

%files -n %{bin_name}
%defattr(-,root,root)
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*
%doc %dir %{_docdir}/%{bin_name}
%license %{_docdir}/%{bin_name}/COPYING*
%dir %{_prefix}/share/icons
%{_prefix}/share/icons/hicolor

%files devel
%defattr(-,root,root)
%dir %{_docdir}/%{bin_name}
%{_libdir}/yui/lib*.so
%{_prefix}/include/yui
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/cmake/%{name}

%changelog
