#
# spec file for package libyui-ncurses-pkg
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

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#


Name:           libyui-ncurses-pkg
Version:        2.50.0
Release:        0
Source:         %{name}-%{version}.tar.bz2

%define so_version 10
%define bin_name %{name}%{so_version}

%if 0%{?suse_version} > 1325
BuildRequires:  libboost_headers-devel
%else
BuildRequires:  boost-devel
%endif
BuildRequires:  cmake >= 2.8
BuildRequires:  gcc-c++
BuildRequires:  libyui-devel >= 3.9.0
BuildRequires:  pkg-config

%define libyui_ncurses_devel_version    libyui-ncurses-devel >= 2.54.0
BuildRequires:  %{libyui_ncurses_devel_version}
%define libzypp_devel_version           libzypp-devel >= 15.11.0
BuildRequires:  %{libzypp_devel_version}

Url:            http://github.com/libyui/
Summary:        Libyui - yast2 package selector widget for the ncurses UI
License:        LGPL-2.1-only OR LGPL-3.0-only
Group:          System/Libraries

%description
This package extends the character based (ncurses) user interface
component for libYUI.


%package -n %{bin_name}

Requires:       libyui%{so_version}
Provides:       %{name} = %{version}

Provides:       yast2-ncurses-pkg = 2.50.0
Obsoletes:      yast2-ncurses-pkg < 2.50.0

# force removal of all previous library versions (bsc#1148622),
# expands to: libyui-ncurses-pkg1 libyui-ncurses-pkg2 ... libyui-ncurses-pkg{so_version - 1}
Obsoletes:      %(echo `seq -s " " -f "libyui-ncurses-pkg%.f" $(expr %{so_version} - 1)`)

Provides:       libyui_pkg
Supplements:    packageand(libyui-ncurses:yast2-packager)

Url:            http://github.com/libyui/
Summary:        Libyui - yast2 package selector widget for the ncurses UI
Group:          System/Libraries

%description -n %{bin_name}
This package extends the character based (ncurses) user interface
component for libYUI.



%package devel

Requires:       %{libyui_ncurses_devel_version}
Requires:       %{libzypp_devel_version}
%if 0%{?suse_version} > 1325
Requires:       libboost_headers-devel
%else
Requires:       boost-devel
%endif
Requires:       %{bin_name} = %{version}
Requires:       glibc-devel
Requires:       libstdc++-devel

Url:            http://github.com/libyui/
Summary:        Libyui-ncurses-pkg header files
Group:          Development/Languages/C and C++

%description devel
This package extends the character based (ncurses) user interface
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

%files devel
%defattr(-,root,root)
%dir %{_docdir}/%{bin_name}
%{_libdir}/yui/lib*.so
%{_prefix}/include/yui
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/cmake/%{name}

%changelog
