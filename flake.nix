{
  description = "C++ project with Wayland + GLFW (NixOS dev shell)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
  };

  outputs = {
    self,
    nixpkgs,
  }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
  in {
    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        cmake
        pkg-config
        gcc

        libGL

        # Wayland + input
        wayland
        wayland-scanner
        wayland-protocols
        libxkbcommon

        # GLFW usually also needs X11 libs
        libX11
        libXcursor
        libXi
        libXrandr
        libxinerama
      ];

      # Optional but helpful for debugging
      shellHook = ''
        export LD_LIBRARY_PATH=${pkgs.lib.makeLibraryPath [
          pkgs.wayland
          pkgs.libxkbcommon
          pkgs.libGL
          pkgs.xorg.libX11
          pkgs.xorg.libXcursor
          pkgs.xorg.libXi
          pkgs.xorg.libXrandr
          pkgs.xorg.libXinerama
        ]}:$LD_LIBRARY_PATH

        echo "Dev shell ready (Wayland + GLFW)"
        pkg-config --modversion wayland-client || true
      '';
    };
  };
}
