with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [ pkg-config ];
  buildInputs = [
    cmake
    gcc
    gdb
    vulkan-headers
    vulkan-loader
    vulkan-tools
    vulkan-validation-layers
    #X11
    xorg.libX11
    xorg.libXrandr
    xorg.libxcb
    xorg.libXinerama
    xorg.libXcursor
    xorg.libXi
    wayland
  ];
}