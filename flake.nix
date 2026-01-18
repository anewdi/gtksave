{
  inputs.nixpkgs.url = "github:nixos/nixpkgs";
  outputs =
    { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      packages.${system}.default = pkgs.callPackage ./package.nix { };

      devShells.${system}.default = pkgs.mkShell {
        inputsFrom = [ self.packages.${system}.default ];

        packages = with pkgs; [
          ruff
        ];

        shellHook = ''
          export XDG_DATA_DIRS=$GSETTINGS_SCHEMAS_PATH
        '';
      };
    };
}
