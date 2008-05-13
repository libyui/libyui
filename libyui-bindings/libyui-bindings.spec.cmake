#
# spec file for package libyui-bindings
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
BuildRequires:  cmake gcc-c++ ruby-devel swig licenses
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
ln -s /usr/share/doc/licenses/GPL-2.0.txt COPYING

%clean
%{__rm} -rf %{buildroot}

%package -n libyui-ruby
Summary:        Ruby bindings for libyui
Group:          Development/Languages/Ruby

%description -n libyui-ruby
-

%files -n libyui-ruby
%defattr(-,root,root,-)
%doc build/COPYING swig/ruby/examples/hello_world.rb swig/ruby/examples/selection_box1.rb
%{_libdir}/ruby/vendor_ruby/%{rb_ver}/%{rb_arch}/yui.so

#%package -n python-yui
#Summary:        Python bindings for libyui
#Group:          Development/Languages/Python
#%description -n python-yui
#-
#
#%files -n python-yui
#%defattr(-,root,root,-)
#%{_libdir}/python2.5/site-packages/_yui.so
#%{_libdir}/python2.5/site-packages/yui.py

#%package -n perl-yui
#Summary:        Perl bindings for libyui
#Group:          Development/Languages/Perl
#
#%description -n perl-yui
#-
#
#%files -n perl-yui
#%defattr(-,root,root,-)
#/usr/lib/perl5/*/*/yui.pm
#/usr/lib/perl5/*/*/yui.so

%changelog
