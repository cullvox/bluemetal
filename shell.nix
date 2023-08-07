{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell 
{
  name = "env";
  packages = with pkgs; [

    # Build requirements
    git-lfs
    pkg-config
    cmake
    gnumake
    gcc
    gdb

    # Vulkan
    vulkan-headers
    vulkan-loader
    vulkan-tools
    vulkan-validation-layers

    # X11
    xorg.libX11
    xorg.libXrandr
    xorg.libxcb
    xorg.libXinerama
    xorg.libXcursor
    xorg.libXi
    
    # Wayland
    wayland
    wayland-protocols
    libxkbcommon
    libffi

    fmt_8
    SDL2
    glm

  ];

  #LD_LIBRARY_PATH = "${lib.makeLibraryPath buildInputs}";
  VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
  XDG_DATA_DIRS = builtins.getEnv "XDG_DATA_DIRS";
  XDG_RUNTIME_DIR = builtins.getEnv "XDG_RUNTIME_DIR";
}