#
# spec file for package libyui-bindings
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
# nodebuginfo


Name:           libyui-bindings

# DO NOT manually bump the version here; instead, use rake version:bump
Version:        4.4.1
Release:        0
Summary:        Bindings for libyui
License:        LGPL-2.1-only OR LGPL-3.0-only
Group:          Development/Sources
URL:            https://github.com/libyui/libyui-bindings
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

BuildRequires:  cmake
BuildRequires:  gcc-c++
# New cmake build environment
BuildRequires:  libyui-devel >= %{version}
BuildRequires:  perl
BuildRequires:  python3-devel
BuildRequires:  ruby-devel
BuildRequires:  swig
Source:         libyui-%{version}.tar.bz2
Prefix:         /usr

%description
This package provides Ruby language bindings to access functions of
libyui - An User Interface engine that provides the
abstraction from graphical user interfaces (Qt, Gtk) and text based
user interfaces (ncurses).

Authors:
---------
-    kkaempf@suse.de
-    dmacvicar@suse.de


%prep
%setup -q -n libyui-%{version}

%build
pushd %{name}
mkdir build
cd build
cmake -DYPREFIX=%{prefix} \
      -DCMAKE_INSTALL_PREFIX=%{prefix} \
      -DLIB=%{_lib} \
      -DPYTHON_SITEDIR=%{python3_sitelib} \
      -DCMAKE_VERBOSE_MAKEFILE=TRUE \
      -DCMAKE_C_FLAGS_RELEASE:STRING="%{optflags}" \
      -DCMAKE_CXX_FLAGS_RELEASE:STRING="%{optflags}" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_SKIP_RPATH=1 \
      -DBUILD_RUBY_GEM=no \
      ..
make %{?jobs:-j %jobs}
popd

%install
pushd %{name}
cd build
make install DESTDIR=$RPM_BUILD_ROOT
popd

%clean
%{__rm} -rf %{buildroot}

%package -n ruby-yui
Summary:        Ruby bindings for libyui
Group:          Development/Languages/Ruby

%description -n ruby-yui
This package provides Ruby language bindings to access functions of
libyui - An User Interface engine that provides the
abstraction from graphical user interfaces (Qt, Gtk) and text based
user interfaces (ncurses).

Authors:
---------
-    kkaempf@suse.de
-    dmacvicar@suse.de


%package -n python3-yui
Summary:        Python 3 bindings for libyui
Group:          Development/Languages/Python

%description -n python3-yui
This package provides Python 3 language bindings to access functions of
libyui - An User Interface engine that provides the
abstraction from graphical user interfaces (Qt, Gtk) and text based
user interfaces (ncurses).

Authors:
---------
-    kkaempf@suse.de
-    dmacvicar@suse.de


%package -n perl-yui
%{perl_requires}
Summary:        Perl bindings for libyui
Group:          Development/Languages/Perl

%description -n perl-yui
This package provides Perl language bindings to access functions of
yast2-libyui - An User Interface engine that provides the
abstraction from graphical user interfaces (Qt, Gtk) and text based
user interfaces (ncurses).

Authors:
---------
-    kkaempf@suse.de
-    dmacvicar@suse.de


%files -n ruby-yui
%defattr(-,root,root,-)
%doc %{name}/swig/ruby/examples/*.rb
%{_libdir}/ruby/vendor_ruby/%{rb_ver}/yui.rb
%{_libdir}/ruby/vendor_ruby/%{rb_ver}/yui
%{_libdir}/ruby/vendor_ruby/%{rb_ver}/%{rb_arch}/_yui.so

%files -n python3-yui
%defattr(-,root,root,-)
%doc %{name}/swig/python/examples/*.py
%{python3_sitelib}/_yui.so
%{python3_sitelib}/yui.py

%files -n perl-yui
%defattr(-,root,root,-)
%doc %{name}/swig/perl/examples/*.pl
%{perl_vendorarch}/yui.so
%{perl_vendorlib}/yui.pm

%changelog
