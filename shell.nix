with import <nixpkgs> {
  overlays = [
    (self: super:
    {
      spdlog = super.spdlog.overrideAttrs (old: {
        src = super.fetchFromGitHub {
          owner = "gabime";
          repo = "spdlog";
          rev = "v1.11.0";
          sha256 = "0i3a1cqrg1sz0w50g7zz9x73rf838igqri12q8ijh4rzpq0qq3ch";
        };
      });
    })
  ];
};


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
    fmt_8 # spdlog has issues with newer versions of fmt
    spdlog
    glfw
    glm
    

  ];
  VULKAN_SDK = "${vulkan-validation-layers}/share/vulkan/explicit_layer.d";
}