{
  inputs.nixpkgs.url = "github:nixos/nixpkgs";
  outputs =
    { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        src = ./src;
        pname = "gtksave";
        version = "0.2";

        nativeBuildInputs = with pkgs; [
          pkg-config
          wrapGAppsHook4
        ];

        buildInputs = with pkgs; [
          gtk4
          glib
        ];

        buildPhase = ''
          NIX_CFLAGS_COMPILE="$(pkg-config --cflags gtk4) $(pkg-config --libs gtk4) $NIX_CFLAGS_COMPILE"
          gcc -o gtksave gtksave.c 
        '';

        installPhase = ''
          mkdir -p $out/bin
          cp gtksave $out/bin
        '';
      };

      devShells.${system}.default = pkgs.mkShell {
        inputsFrom = [ self.packages.${system}.default ];

        packages = with pkgs; [
          gtk4
          glib
          pkg-config
          clang-tools
          bear
        ];
        shellHook = ''
          export XDG_DATA_DIRS=$GSETTINGS_SCHEMAS_PATH
        '';
      };
    };
}
