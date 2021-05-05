Name:		bce2
Version:	0.0.3
Release:	1%{?dist}
License:	GPLv3
Summary:	BTC blockchain export
URL:		https://github.com/tieugene/bce2
Source:		https://github.com/tieugene/bce2/archive/%{version}.tar.gz/%{name}-%{version}.tar.gz
BuildRequires:	gcc-c++
BuildRequires:	cmake
# openssl-devel
BuildRequires:	pkgconfig(libcrypto)
# kyotocabinet-devel
BuildRequires:	pkgconfig(kyotocabinet)
# leveldb-devel
BuildRequires:	cmake(leveldb)
# for tools/btcbklocs.py
Recommends:     python3-plyvel


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
%doc README.md doc/*
%{_bindir}/%{name}
%{_bindir}/btcbklocs
%{_bindir}/btclocview


%changelog
* Thu May 06 2021 TI_Eugene <ti.eugene@gmail.com> 0.0.3-1
- Version bump

* Sat Feb 20 2021 TI_Eugene <ti.eugene@gmail.com> 0.0.2-1
- Version bump

* Mon Sep 28 2020 TI_Eugene <ti.eugene@gmail.com> 0.0.1-1
- Initial packaging
