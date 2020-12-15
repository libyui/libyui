#
# spec file for package libyui-qt-doc
#
# Copyright (c) 2014-2019 SUSE LINUX Products GmbH, Nuernberg, Germany.
# Copyright (c) 2020 SUSE LLC, Nuernberg, Germany.
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


%define parent libyui-qt
%define so_version 14

Name:           %{parent}-doc
# DO NOT manually bump the version here; instead, use   rake version:bump
Version:        2.56.4
Release:        0
Source:         %{parent}-%{version}.tar.bz2

BuildArch:      noarch

BuildRequires:  cmake >= 3.10
BuildRequires:  doxygen
BuildRequires:  graphviz
BuildRequires:  fdupes
BuildRequires:  gcc-c++
BuildRequires:  libyui-devel >= 3.8.0

Url:            http://github.com/libyui/
Summary:        Libyui-qt documentation
License:        LGPL-2.1 or LGPL-3.0
Group:          Documentation/HTML

%description
This package contains the Qt user interface component for libYUI.

This package provides HTML class documentation.


%prep

%setup -n %{parent}-%{version}

%build

mkdir build
cd build

cmake .. \
  -DBUILD_DOC=on \
  -DBUILD_SRC=off \
  -DDOC_DESTDIR=$RPM_BUILD_ROOT

make %{?jobs:-j%jobs} doc


%install
cd build
make install-doc

%fdupes -s $RPM_BUILD_ROOT/%_docdir/%{parent}%{so_version}

%clean
rm -rf "$RPM_BUILD_ROOT"

%files
%defattr(-,root,root)
%doc %{_docdir}/%{parent}%{so_version}

%changelog
