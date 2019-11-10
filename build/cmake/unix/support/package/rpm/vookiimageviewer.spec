Summary: Cross-platform lightweight image viewer for a fast image preview
Url: https://github.com/vookimedlo/vooki-image-viewer
Name: vookiimageviewer
Group: Applications/Multimedia
Version: 2019.11.10
Release: 1%{?dist}
License: GPLv3+
BuildRoot: %{_tmppath}/%name-%version-%release
Packager: Michal Duda <github@vookimedlo.cz>
Vendor: https://github.com/vookimedlo/vooki-image-viewer
Prefix: %{_prefix}

Source0: %{name}-%{version}.tar.bz2

# gcc, gcc-c++, clang packages are omitted intentionally, because we don't know if clang or gcc is installed
# and we don't require to be both installed at the same time
#
BuildRequires: LibRaw-devel, cmake, git, make, qt5, qt5-qtbase-devel, desktop-file-utils
Suggests: qt5-qtimageformats

%description
Cross-platform lightweight image viewer for a fast image preview.
Lightweight image viewer for a fast image preview. It has been developed to have
the same viewer available for all major operating systems - Windows 10, MacOS and
GNU/Linux.

The main goal is to have a free of charge cross-platform viewer with a simple design
and minimum functions which are commonly used.

# Do not list our plugins in a provide list, since the are intended to be private
# See https://docs.fedoraproject.org/en-US/packaging-guidelines/AutoProvidesAndRequiresFiltering/
#
%global __provides_exclude_from ^%{_libdir}/%{name}/imageformats/.*\\.so.*$

%prep
rm -rf %{buildroot}
%setup -qn vooki-image-viewer

%build
export CXXFLAGS="${RPM_OPT_FLAGS} -Wno-deprecated"
export CFLAGS="${RPM_OPT_FLAGS}"

cmake -DLIB_INSTALL_DIR=%{_lib} -DCMAKE_INSTALL_PREFIX:PATH=/usr -H. -Bbuild-rpm build/cmake
cmake --build build-rpm --config Release %{_smp_mflags}

%install
make -C build-rpm install DESTDIR="${RPM_BUILD_ROOT}"
desktop-file-validate %{buildroot}/%{_datadir}/applications/vookiimageviewer.desktop

%clean
rm -rf "${RPM_BUILD_ROOT}"

%files
%attr(755, root, root) %{_bindir}/VookiImageViewer
%defattr(-, root, root, -)
%doc LICENSE LICENSE-IMAGES LICENSE-OPENCLIPART LICENSE-PLUGINS-KIMAGEFORMATS README.md
%dir %{_libdir}/%{name}
%dir %{_libdir}/%{name}/imageformats
%{_libdir}/%{name}/imageformats/*.so
%{_datadir}/applications/vookiimageviewer.desktop
%{_datadir}/pixmaps/vookiimageviewericon.png

