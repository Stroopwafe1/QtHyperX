{ stdenv, lib, qt6, cmake, hidapi, pkg-config, kdePackages, ... }:
let
  fs = lib.fileset;
  sourceFiles = fs.unions [
    ./src
    ./CMakeLists.txt
    ./android
    ./LICENSES
    ./org.kde.qthyperx.metainfo.xml
    ./org.kde.qthyperx.desktop
  ];
in
stdenv.mkDerivation {
 pname = "QTHyperX";
 version = "1.0";
 src = fs.toSource {
   root = ./.;
   fileset = sourceFiles;
 };
 buildInputs = [
   qt6.qtbase
   qt6.qtsvg
   qt6.qttools
   kdePackages.kirigami
   kdePackages.kirigami-addons
   hidapi
 ];
 nativeBuildInputs = [
   cmake
   qt6.wrapQtAppsHook
   pkg-config
 ];

 installPhase = ''
   mkdir -p $out/bin
   cp -a bin/. $out/bin/
 '';

 postInstall = ''
   mkdir -p $out/etc/udev/rules.d/
   echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="03f0", ATTRS{idProduct}=="098d", MODE="660"' >> $out/etc/udev/rules.d/69-hid.rules
   echo 'KERNEL=="hidraw*", ATTRS{idVendor}=="03f0", ATTRS{idProduct}=="098d", MODE="0666", TAG+="systemd", TAG+="uaccess"' >> $out/etc/udev/rules.d/69-hid.rules
 '';
}
