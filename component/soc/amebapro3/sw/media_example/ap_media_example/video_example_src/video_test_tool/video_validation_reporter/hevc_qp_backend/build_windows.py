"""Explicitly build the Windows PC QP decoder (never invoked by the analyzer)."""
import json
import os
from pathlib import Path
import shutil
import subprocess

base = Path(__file__).resolve().parent
vswhere = Path(os.environ.get("ProgramFiles(x86)", "C:/Program Files (x86)")) / "Microsoft Visual Studio/Installer/vswhere.exe"
if not vswhere.is_file():
    raise SystemExit("Install Visual Studio / Build Tools with Desktop development with C++ (see README).")
info = json.loads(subprocess.check_output([str(vswhere), "-latest", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-format", "json", "-utf8"], encoding="utf-8"))
if not info:
    raise SystemExit("Visual Studio C++ x64 tools are required")
vs = Path(info[0]["installationPath"])
setup = vs / "VC/Auxiliary/Build/vcvars64.bat"
command = 'cmd.exe /d /s /c "call "' + str(setup) + '" >nul && set"'
result = subprocess.run(command, capture_output=True, text=True, errors="replace", check=True)
env = {k.upper(): v for k, v in os.environ.items()}
for line in result.stdout.splitlines():
    if "=" in line:
        key, value = line.split("=", 1)
        env[key.upper()] = value
cmake = vs / "Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
ninja = vs / "Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe"
compiler_path = shutil.which("cl", path=env["PATH"])
if not compiler_path or not cmake.is_file() or not ninja.is_file():
    raise SystemExit("Install the Visual Studio C++ compiler and C++ CMake tools for Windows.")
compiler = Path(compiler_path)
build = base / "build/windows"
subprocess.run([str(cmake), "-S", str(base), "-B", str(build), "-G", "Ninja",
                "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_MAKE_PROGRAM=" + ninja.as_posix(),
                "-DCMAKE_C_COMPILER=" + compiler.as_posix(),
                "-DCMAKE_CXX_COMPILER=" + compiler.as_posix()], env=env, check=True)
# Native localized /showIncludes output can be very verbose; retain it in the build log.
log = build / "build.log"
with log.open("wb") as output:
    result = subprocess.run([str(cmake), "--build", str(build), "--target", "de265", "--parallel", "4"],
                            env=env, stdin=subprocess.DEVNULL, stdout=output, stderr=subprocess.STDOUT)
if result.returncode:
    raise SystemExit(f"PC decoder build failed; see {log}")
# Keep the analyzer's default discovery path independent of the generator.
dest = build / "bin"
dest.mkdir(exist_ok=True)

print(dest / "libde265_qp.dll")
