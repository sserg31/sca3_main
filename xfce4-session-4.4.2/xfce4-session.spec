Summary: 	Xfce Session manager
Name: 		xfce4-session
Version: 	4.4.2
Release: 	1
License:	GPL
URL: 		http://www.xfce.org/
Source0: 	%{name}-%{version}.tar.gz
Group: 		User Interface/Desktops
BuildRoot: 	%{_tmppath}/%{name}-root
Requires:	libxfcegui4 >= 4.4.0
Requires:	libxfce4mcs >= 4.4.0
Requires:	xfce-mcs-manager >= 4.4.0
BuildRequires: 	libxfcegui4-devel >= 4.4.0
BuildRequires:	libxfce4mcs-devel >= 4.4.0
BuildRequires:	xfce-mcs-manager-devel >= 4.4.0

%description
xfce4-session is the session manager for the Xfce desktop environment.

%package devel
Summary:	Development files for xfce4-session
Group:		Development/Libraries
Requires:	xfce4-session >= 4.4.2

%description devel
Header files for the Xfce Session Manager.

%package engines
Summary:	Additional engines for xfce4-session
Group:		User Interface/Desktops
Requires:	xfce4-session >= 4.4.2

%description engines
Additional splash screen engines for the Xfce Session Manager.

%prep
%setup -q

%build
%configure --enable-final
make

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT mandir=%{_mandir}

%clean
rm -rf $RPM_BUILD_ROOT

%post
touch --no-create %{_datadir}/icons/hicolor || :
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
   %{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
fi

%postun
touch --no-create %{_datadir}/icons/hicolor || :
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
   %{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
fi

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog ChangeLog.pre-xfce-devel NEWS README TODO
%{_bindir}/*
%{_datadir}/applications/*
%{_datadir}/icons/*
%{_datadir}/xfce4/*
%{_datadir}/locale/*
%{_libdir}/lib*.so.*
%{_libdir}/xfce4/mcs-plugins/*
%{_libdir}/xfce4/splash/engines/libmice.*
%{_libexecdir}/xfsm-shutdown-helper
%{_mandir}/man1/*
%{_sysconfdir}/xdg/*

%files devel
%defattr(-,root,root)
%{_includedir}/xfce4/xfce4-session-4.2/libxfsm/*.h
%{_libdir}/lib*.so
%{_libdir}/*a
%{_libdir}/pkgconfig/*.pc

%files engines
%defattr(-,root,root)
%{_datadir}/themes/Default/balou/*
%{_libdir}/xfce4/splash/engines/libbalou.*
%{_libdir}/xfce4/splash/engines/libsimple.*
%{_libexecdir}/balou-*
