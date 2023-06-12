with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [ pkg-config ];

  buildInputs = [
    # Build requirements
    cmake
    gcc
    gdb

    # Vulkan
    vulkan-headers
    vulkan-loader
    vulkan-tools
    vulkan-validation-layers

    # X11 (on god, wheres X12)
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

    # Miscellanious libraries
    fmt_8
    SDL2
    glm
  ];

  #LD_LIBRARY_PATH = "${lib.makeLibraryPath buildInputs}";
  VK_LAYER_PATH = "${vulkan-validation-layers}/share/vulkan/explicit_layer.d";
  XDG_DATA_DIRS = builtins.getEnv "XDG_DATA_DIRS";
  XDG_RUNTIME_DIR = builtins.getEnv "XDG_RUNTIME_DIR";
}