#
# spec file for package libyui-bindings
#
# Copyright (c) 2020 SUSE LINUX GmbH, Nuernberg, Germany.
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
# nodebuginfo


Name:           libyui-bindings
Version:        2.0.2
Release:        0
Summary:        Bindings for libyui
License:        LGPL-2.1-only OR LGPL-3.0-only
Group:          Development/Sources
Url:            https://github.com/libyui/libyui-bindings
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  libyui-devel >= 2.21.5
BuildRequires:  perl
BuildRequires:  python3-devel
BuildRequires:  ruby-devel
BuildRequires:  swig
Source:         %{name}-%{version}.tar.bz2
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
%setup -q

%build
mkdir build
cd build
cmake -DYPREFIX=%{prefix} \
      -DCMAKE_INSTALL_PREFIX=%{prefix} \
      -DLIB=%{_lib} \
      -DPYTHON_SITEDIR=%{python_sitelib} \
      -DCMAKE_VERBOSE_MAKEFILE=TRUE \
      -DCMAKE_C_FLAGS_RELEASE:STRING="%{optflags}" \
      -DCMAKE_CXX_FLAGS_RELEASE:STRING="%{optflags}" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_SKIP_RPATH=1 \
      -DBUILD_RUBY_GEM=no \
      ..
make %{?jobs:-j %jobs}

%install
cd build
make install DESTDIR=$RPM_BUILD_ROOT

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
%doc swig/ruby/examples/*.rb
%{_libdir}/ruby/vendor_ruby/%{rb_ver}/yui.rb
%{_libdir}/ruby/vendor_ruby/%{rb_ver}/yui
%{_libdir}/ruby/vendor_ruby/%{rb_ver}/%{rb_arch}/_yui.so

%files -n python3-yui
%defattr(-,root,root,-)
%doc swig/python/examples/*.py
%{python_sitelib}/_yui.so
%{python_sitelib}/yui.py

%files -n perl-yui
%defattr(-,root,root,-)
%doc swig/perl/examples/*.pl
%{perl_vendorarch}/yui.so
%{perl_vendorlib}/yui.pm

%changelog
