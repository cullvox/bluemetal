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
    #fmt_8 # spdlog has issues with newer versions of fmt
    spdlog_1
    SDL2
    glm

  ];
  VULKAN_SDK = "${vulkan-validation-layers}/share/vulkan/explicit_layer.d";
}