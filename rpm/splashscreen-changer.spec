Name:       splashscreen-changer

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}

Summary:    Splashscreen changer
Version:    0.2.0
Release:    1
Group:      Qt/Qt
License:    LICENSE
URL:        http://example.org/
Source0:    %{name}-%{version}.tar.bz2
Source100:  splashscreen-changer.yaml
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   droid-config-l500d-sailfish
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils

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
%{_datadir}/splashscreen-changer/data/*.header
%{_datadir}/splashscreen-changer/icons/*.png
%{_datadir}/splashscreen-changer/qml/*.qml
%{_datadir}/jolla-settings/entries/*.json
%{_datadir}/dbus-1/services/*.service
