Name: loki
Summary: Loki C++ Library
Version: 0.1.4
Release: 1
License: MIT License
Group: Productivity/Development
URL: http://prdownloads.sourceforge.net/loki-lib/loki-0.1.4.tar.bz2
Distribution: SuSE 10.0 (i586)
Source0: %{name}-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-root
BuildArch: i586

%description
A C++ library of designs, containing flexible implementations of common design
patterns and idioms.

%prep
%setup -q

%build
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/include
cp -a include/loki $RPM_BUILD_ROOT/usr/include
mkdir -p $RPM_BUILD_ROOT/usr/lib
cp lib/libloki.a $RPM_BUILD_ROOT/usr/lib/libloki.a
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/loki-%{version}
cp -a doc/{flex,html,yasli} $RPM_BUILD_ROOT/usr/share/doc/loki-%{version}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(644,root,root,755)
/usr/include/loki
/usr/lib/libloki.a
/usr/share/doc/loki-%{version}

%post

%postun

%changelog
* Mon Jan 06 2006 Andreas Scherer <andreas_hacker@freenet.de>
- Initial build
