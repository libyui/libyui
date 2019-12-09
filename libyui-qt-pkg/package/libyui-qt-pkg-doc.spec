#
# spec file for package libyui-qt-pkg-doc
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

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#


%define parent libyui-qt-pkg
%define so_version 10
Name:           %{parent}-doc
Version:        2.46.11
Release:        0
Summary:        Libyui-qt-pkg documentation
License:        LGPL-2.1-only OR LGPL-3.0-only
URL:            https://github.com/libyui/
Source:         %{parent}-%{version}.tar.bz2
BuildRequires:  cmake >= 2.8
BuildRequires:  doxygen
BuildRequires:  fdupes
BuildRequires:  gcc-c++
BuildRequires:  graphviz
BuildRequires:  graphviz-gnome
BuildRequires:  libyui-devel >= 3.0.4
BuildRequires:  texlive-epstopdf-bin
BuildRequires:  texlive-latex
BuildArch:      noarch

%description
This package contains the Qt package selector
component for libYUI.

This package provides the documentation. (HTML & PDF)

%prep
%setup -q -n %{parent}-%{version}

%build
export CFLAGS="%{optflags} -DNDEBUG"
export CXXFLAGS="%{optflags} -DNDEBUG"

./bootstrap.sh %{_prefix}

%cmake \
  -DDOC_DIR=%{_docdir} \
  -DDOCS_ONLY=ON
%cmake_build docs

%install
%cmake_install
%fdupes -s %{buildroot}/%{_docdir}/%{parent}%{so_version}

%files
%doc %{_docdir}/%{parent}%{so_version}

%changelog
