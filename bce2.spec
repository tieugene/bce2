Name:		bce2
Version:	0.0.1
Release:	1%{?dist}
License:	GPLv3
Summary:	BTC blockchain export
Source:		%{name}-%{version}.tar.bz2
BuildRequires:	gcc-c++
BuildRequires:	cmake
# openssl-devel
BuildRequires:	pkgconfig(libcrypto)
# kyotocabinet-devel
BuildRequires:	pkgconfig(kyotocabinet)
# leveldb-devel
BuildRequires:	cmake(leveldb)


%description
BitCoin Export 2 is tool to export BTC blockchain into SQL loadable files.


%prep
%autosetup


%build
%{cmake}
%{cmake_build}


%install
%{cmake_install}


%files
%license LICENSE
%doc README.md
%{_bindir}/%{name}
%{_bindir}/btcbklocs


%changelog
* Mon Sep 28 2020 TI_Eugene <ti.eugene@gmail.com> 0.0.1-1
- Initial packaging
