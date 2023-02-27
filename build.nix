{ pkgs } :
pkgs.stdenv.mkDerivation rec {
    pname = "darkred";
    version = "0.1.0";

    src = pkgs.fetchgit {
        url = "https://github.com/cadenmiller/DarkRed";
        rev = "
    }
}