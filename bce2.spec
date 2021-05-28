Name:		bce2
Version:	0.1.0
Release:	0.1%{?dist}
License:	GPLv3
Summary:	BTC blockchain export
URL:		https://github.com/tieugene/bce2
Source:		https://github.com/tieugene/bce2/archive/%{version}.tar.gz/%{name}-%{version}.tar.gz
BuildRequires:	gcc-c++
BuildRequires:	cmake
BuildRequires:	asciidoctor
# openssl-devel
BuildRequires:	pkgconfig(libcrypto)
# kyotocabinet-devel
BuildRequires:	pkgconfig(kyotocabinet)
# tkrzw-devel
BuildRequires:	pkgconfig(tkrzw) >= 0.9.9
BuildRequires:	libdb-cxx-devel
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
make -C doc


%install
%{cmake_install}
# mans
install -d %{buildroot}%{_mandir}/{man1,man5}
install -t %{buildroot}%{_mandir}/man1 -m 0644 -p doc/*.1
install -t %{buildroot}%{_mandir}/man5 -m 0644 -p doc/*.5


%files
%license LICENSE
%doc README.md doc/*.md
%{_bindir}/%{name}
%{_bindir}/btcbklocs
%{_bindir}/btclocview
%{_mandir}/man{1,5}/*


%changelog
* Mon May 28 2021 TI_Eugene <ti.eugene@gmail.com> 0.1.0-0.1
- Version bump, pre.1 (bk classed, chk addr)

* Fri May 28 2021 TI_Eugene <ti.eugene@gmail.com> 0.0.5-1
- Version bump
- hex input speedup
- multiengine k-v

* Tue May 11 2021 TI_Eugene <ti.eugene@gmail.com> 0.0.4-1
- Version bump (hex speedup, tkrzw)

* Thu May 06 2021 TI_Eugene <ti.eugene@gmail.com> 0.0.3-1
- Version bump (hex)

* Sat Feb 20 2021 TI_Eugene <ti.eugene@gmail.com> 0.0.2-1
- Version bump

* Mon Sep 28 2020 TI_Eugene <ti.eugene@gmail.com> 0.0.1-1
- Initial packaging
