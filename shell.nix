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
    kdePackages.kirigami
    kdePackages.kirigami-addons
    kdePackages.full
    pkgconf
    hidapi
    qt6.full
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
    addToSearchPath QT_QPA_PLATFORM_PLUGIN_PATH "${kdePackages.qtbase.out}/lib/qt-${qt6.qtbase.version}/plugins"
  '';
}
