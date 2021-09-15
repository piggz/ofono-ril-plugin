Name: ofono-ril-plugin
Version: 1.0.0
Release: 1
Summary: Ofono legacy RIL plugin
License: GPLv2
URL: https://github.com/mer-hybris/ofono-ril-plugin
Source: %{name}-%{version}.tar.bz2

%define libgrilio_version 1.0.35
%define libglibutil_version 1.0.49
%define libmce_version 1.0.6

# API appeared in ofono 1.24+git2, built-in plugin was removed in 1.24+git4
Requires: ofono >= 1.24+git4
Requires: libgrilio >= %{libgrilio_version}
Requires: libglibutil >= %{libglibutil_version}
Requires:   libmce-glib >= %{libmce_version}
BuildRequires: ofono-devel >= 1.24+git2
BuildRequires: pkgconfig(libgrilio) >= %{libgrilio_version}
BuildRequires:  pkgconfig(libglibutil) >= %{libglibutil_version}
BuildRequires:  pkgconfig(libmce-glib) >= %{libmce_version}

%define plugin_dir %{_libdir}/ofono/plugins
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
make LIBDIR=%{_libdir} DESTDIR=%{buildroot} PLUGINDIR=%{plugin_dir} install
mkdir -p %{buildroot}%{config_dir}
install -m 644 ril_subscription.conf %{buildroot}%{config_dir}

%files
%dir %{plugin_dir}
%defattr(-,root,root,-)
%{plugin_dir}/rilplugin.so

%files -n ofono-configs-mer
%dir %{config_dir}
%defattr(-,root,root,-)
%config %{config_dir}/ril_subscription.conf
