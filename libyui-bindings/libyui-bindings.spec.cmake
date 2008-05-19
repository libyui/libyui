#
# spec file for package libyui-bindings
# generates:
#  libyui-ruby
#  libyui-python
#  perl-libyui (Perl naming convention)
#
# Copyright (c) 2007 SUSE LINUX Products GmbH, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# nodebuginfo

Name:           @PACKAGE@
Version:        @VERSION@
Release:        0
License:        GPL
Summary:        Bindings for libyui
Group:          Development/Sources
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires:  cmake gcc-c++ ruby-devel perl python-devel swig
BuildRequires:  yast2-libyui-devel >= 2.16
Source:         %{name}-%{version}.tar.bz2

%description
-

%prep
%setup -q

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=%{prefix} \
      -DLIB=%{_lib} \
      -DCMAKE_VERBOSE_MAKEFILE=TRUE \
      -DCMAKE_C_FLAGS_RELEASE:STRING="%{optflags}" \
      -DCMAKE_CXX_FLAGS_RELEASE:STRING="%{optflags}" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_SKIP_RPATH=1 \
      ..
make %{?jobs:-j %jobs}

%install
cd build
make install DESTDIR=$RPM_BUILD_ROOT

%clean
%{__rm} -rf %{buildroot}

%package -n libyui-ruby
Summary:        Ruby bindings for yast2-libyui
Group:          Development/Languages/Ruby

%description -n libyui-ruby
-

%package -n libyui-python
Summary:        Python bindings for yast2-libyui
Group:          Development/Languages/Python

%description -n libyui-python
-

%package -n perl-libyui
Requires:       perl-base >= 5.10
Summary:        Perl bindings for yast2-libyui
Group:          Development/Languages/Perl

%description -n perl-libyui
-

%files -n libyui-ruby
%defattr(-,root,root,-)
%doc swig/ruby/examples/*.rb
%{_libdir}/ruby/vendor_ruby/%{rb_ver}/%{rb_arch}/yui.so

%files -n libyui-python
%defattr(-,root,root,-)
%doc swig/python/examples/*.py
%{_libdir}/python2.5/site-packages/_yui.so
%{_libdir}/python2.5/site-packages/yui.py

%files -n perl-libyui
%defattr(-,root,root,-)
%doc swig/perl/examples/*.pl
/usr/lib/perl5/vendor_perl/*/*/yui.so
/usr/lib/perl5/vendor_perl/*/yui.pm

%changelog
