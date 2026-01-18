{
  wrapGAppsHook3,
  gtk4,
  gobject-introspection,
  glib,
  python3Packages,
}:

python3Packages.buildPythonApplication rec {
  pname = "gtksave";
  version = "0.0.1";
  pyproject = true;

  src = ./.;

  nativeBuildInputs = [
    wrapGAppsHook3
    gobject-introspection
  ];

  buildInputs = [
    glib
    gtk4
  ];

  build-system = with python3Packages; [ setuptools ];

  propagatedBuildInputs = with python3Packages; [
    pygobject3
  ];

  postFixup = ''
    mv $out/bin/${pname}.py $out/bin/${pname}
  '';

  meta = {
    description = "Uses gtk file dialog to save data from stdin";
    mainProgram = "gtksave";
  };
}
