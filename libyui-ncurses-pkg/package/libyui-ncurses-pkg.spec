#
# spec file for package libyui-ncurses-pkg
#
# Copyright (c) 2015 SUSE LINUX GmbH, Nuernberg, Germany.
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


Name:           libyui-ncurses-pkg
Version:        2.47.0
Release:        0
Source:         libyui-ncurses-pkg-%{version}.tar.bz2

BuildRequires:  boost-devel
BuildRequires:  cmake >= 2.8
BuildRequires:  gcc-c++
BuildRequires:  libyui-devel >= 3.0.4
BuildRequires:  pkg-config

%define libyui_ncurses_devel_version    libyui-ncurses-devel >= 2.43.9
BuildRequires:  %{libyui_ncurses_devel_version}
%define libzypp_devel_version           libzypp-devel >= 12.3.0
BuildRequires:  %{libzypp_devel_version}

Url:            http://github.com/libyui/
Summary:        Libyui - yast2 package selector widget for the ncurses UI
License:        LGPL-2.1 or LGPL-3.0
Group:          System/Libraries

%description
This package extends the character based (ncurses) user interface
component for libYUI.


%package -n libyui-ncurses-pkg7

Requires:       libyui7
Provides:       libyui-ncurses-pkg = %{version}

Provides:       yast2-ncurses-pkg = 2.42.0
Obsoletes:      yast2-ncurses-pkg < 2.42.0

Provides:       libyui_pkg
Supplements:    packageand(libyui-ncurses:yast2-packager)

Url:            http://github.com/libyui/
Summary:        Libyui - yast2 package selector widget for the ncurses UI
Group:          System/Libraries

%description -n libyui-ncurses-pkg7
This package extends the character based (ncurses) user interface
component for libYUI.



%package devel

Requires:       %{libyui_ncurses_devel_version}
Requires:       %{libzypp_devel_version}
Requires:       boost-devel
Requires:       glibc-devel
Requires:       libstdc++-devel
Requires:       libyui-ncurses-pkg7 = %{version}

Url:            http://github.com/libyui/
Summary:        Libyui-ncurses-pkg header files
Group:          Development/Languages/C and C++

%description devel
This package extends the character based (ncurses) user interface
component for libYUI.


This can be used independently of YaST for generic (C++) applications.
This package has very few dependencies.


%prep
%setup -q -n libyui-ncurses-pkg-%{version}

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
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/libyui-ncurses-pkg7/
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0644 ../COPYING* $RPM_BUILD_ROOT/%{_docdir}/libyui-ncurses-pkg7/

%clean
rm -rf "$RPM_BUILD_ROOT"

%post -n libyui-ncurses-pkg7 -p /sbin/ldconfig

%postun -n libyui-ncurses-pkg7 -p /sbin/ldconfig

%files -n libyui-ncurses-pkg7
%defattr(-,root,root)
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*
%doc %dir %{_docdir}/libyui-ncurses-pkg7
%doc %{_docdir}/libyui-ncurses-pkg7/COPYING*

%files devel
%defattr(-,root,root)
%dir %{_docdir}/libyui-ncurses-pkg7
%{_libdir}/yui/lib*.so
%{_prefix}/include/yui
%{_libdir}/pkgconfig/libyui-ncurses-pkg.pc
%{_libdir}/cmake/libyui-ncurses-pkg

%changelog
