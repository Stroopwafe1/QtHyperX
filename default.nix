{ pkgs ? import <nixpkgs> {} }:
pkgs.qt6Packages.callPackage ./package.nix {}
