#
# spec file for package libyui-qt-pkg-doc
#
# Copyright (c) 2014-2019 SUSE LINUX Products GmbH, Nuernberg, Germany.
# Copyright (c) 2020-2021 SUSE LLC, Nuernberg, Germany.
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


%define parent libyui-qt-pkg
%define so_version 14

Name:           %{parent}-doc
# DO NOT manually bump the version here; instead, use   rake version:bump
Version:        2.48.5
Release:        0
Source:         %{parent}-%{version}.tar.bz2

BuildArch:      noarch
Summary:        Libyui-qt-pkg documentation
License:        LGPL-2.1-only OR LGPL-3.0-only
URL:            https://github.com/libyui/

BuildRequires:  cmake >= 3.10
BuildRequires:  doxygen
BuildRequires:  graphviz
BuildRequires:  fdupes
BuildRequires:  gcc-c++
BuildRequires:  libyui-devel >= 3.9.0

%description
This package contains the Qt package selector component for libYUI.

This package provides HTML class documentation.


%prep
%setup -q -n %{parent}-%{version}

%build

mkdir build
cd build

cmake .. \
  -DBUILD_DOC=on \
  -DBUILD_SRC=off \
  -DDOC_DESTDIR=$RPM_BUILD_ROOT

# No "make doc" here: This would only duplicate the doxygen call


%install

cd build
make install-doc
# This implicitly includes "make doc" unconditionally

%fdupes -s %{buildroot}/%{_docdir}/%{parent}%{so_version}


%files
%defattr(-,root,root)
%doc %{_docdir}/%{parent}%{so_version}

%changelog
