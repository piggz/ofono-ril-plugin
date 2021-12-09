Name: ofono-ril-plugin

Version: 1.0.5
Release: 1
Summary: Ofono legacy RIL plugin
License: GPLv2
URL: https://github.com/mer-hybris/ofono-ril-plugin
Source: %{name}-%{version}.tar.bz2

%define ofono_version 1.25+git7
%define libgrilio_version 1.0.35
%define libglibutil_version 1.0.49
%define libmce_version 1.0.6

Requires: ofono >= %{ofono_version}
Requires: libgrilio >= %{libgrilio_version}
Requires: libglibutil >= %{libglibutil_version}
Requires: libmce-glib >= %{libmce_version}

BuildRequires: pkgconfig
BuildRequires: ofono-devel >= %{ofono_version}
BuildRequires: pkgconfig(libgrilio) >= %{libgrilio_version}
BuildRequires: pkgconfig(libglibutil) >= %{libglibutil_version}
BuildRequires: pkgconfig(libmce-glib) >= %{libmce_version}

# license macro requires rpm >= 4.11
BuildRequires: pkgconfig(rpm)
%define license_support %(pkg-config --exists 'rpm >= 4.11'; echo $?)

%define plugin_dir %(pkg-config ofono --variable=plugindir)
%define config_dir /etc/ofono/

%description
This package contains ofono plugin which suppors legacy RIL sockets

%package -n ofono-configs-mer
Summary:    Package to provide default configs for ofono
Provides:   ofono-configs

%description -n ofono-configs-mer
This package provides default configs for ofono

%prep
%setup -q -n %{name}-%{version}

%build
make %{_smp_mflags} LIBDIR=%{_libdir} KEEP_SYMBOLS=1 release

%check
make test

%install
rm -rf %{buildroot}
make LIBDIR=%{_libdir} DESTDIR=%{buildroot} install
mkdir -p %{buildroot}%{config_dir}
install -m 644 ril_subscription.conf %{buildroot}%{config_dir}

%files
%dir %{plugin_dir}
%defattr(-,root,root,-)
%{plugin_dir}/rilplugin.so
%if %{license_support} == 0
%license LICENSE
%endif

%files -n ofono-configs-mer
%dir %{config_dir}
%defattr(-,root,root,-)
%config %{config_dir}/ril_subscription.conf
