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
  ];
}