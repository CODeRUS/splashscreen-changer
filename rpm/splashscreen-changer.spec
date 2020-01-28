%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}

Name:       splashscreen-changer

Summary:    Splashscreen changer
Version:    0.3.0
Release:    1
Group:      Qt/Qt
License:    LICENSE
URL:        http://example.org/
Source0:    %{name}-%{version}.tar.bz2
Source100:  splashscreen-changer.yaml
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(zlib)
BuildRequires:  desktop-file-utils
BuildRequires:  sailfish-svg2png >= 0.1.5
BuildRequires:  qt5-qttools-linguist

%description
Short description of my SailfishOS Application


%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5 

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install

%pre
if /sbin/pidof splashscreen-flasher > /dev/null; then
killall splashscreen-flasher || true
fi

%preun
if /sbin/pidof splashscreen-flasher > /dev/null; then
killall splashscreen-flasher || true
fi

%files
%attr(4755, root, root) %{_bindir}/splashscreen-flasher
%defattr(-,root,root,-)
%{_bindir}/splashscreen-changer
%{_datadir}/applications/splashscreen-changer.desktop
%{_datadir}/splashscreen-changer/*
%{_datadir}/dbus-1/services/*.service
%{_datadir}/icons/hicolor/*/apps/%{name}.png
