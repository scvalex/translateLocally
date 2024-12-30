{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        overlays = [ ];
        pkgs = import nixpkgs { inherit system overlays; };
      in
      rec {
        # `nix develop`
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            difftastic
            pre-commit
            tokei
            just
            git-crypt

            clang-tools
            cmake
            libarchive
            qt6.qttools
            qt6.qtbase
            qt6.qtsvg
            qt6.qtwayland
          ];

          GIT_EXTERNAL_DIFF = "${pkgs.difftastic}/bin/difft";
        };
      }
    );
}
