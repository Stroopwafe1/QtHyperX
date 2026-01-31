{ pkgs ? import <nixpkgs> { } }:
with pkgs;
mkShell {
  name = "qthyperx";

  buildInputs = [
    libGL
    mesa
    cmake
    gnumake
    gcc
    libgcc
    kdePackages.kirigami
    kdePackages.kirigami-addons
    kdePackages.extra-cmake-modules
    pkgconf
    hidapi
    qt6.qtbase
    qtcreator
    qt6.wrapQtAppsHook
    makeWrapper
    bashInteractive
    kdePackages.kdevelop
  ];

  # This creates the proper qt env so that plugins are found right.
  shellHook = ''
    bashdir=$(mktemp -d)
    makeWrapper "$(type -p bash)" "$bashdir/bash" "''${qtWrapperArgs[@]}"
    exec "$bashdir/bash"
    addToSearchPath QT_QPA_PLATFORM_PLUGIN_PATH "${kdePackages.qtbase.out}/lib/qt-${qt6.qtbase.version}/plugins:${kdePackages.kirigami-addons.out}/lib/qt-${qt6.qtbase.version}/"
    export PATH="$PATH:${pkgs.gcc}/bin:${pkgs.cmake}/bin"
  '';
}
