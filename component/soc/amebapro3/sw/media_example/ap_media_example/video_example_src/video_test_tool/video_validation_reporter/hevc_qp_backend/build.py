"""Build the platform-specific host decoder; never builds SDK firmware."""
from pathlib import Path
import subprocess
import sys

base = Path(__file__).resolve().parent
if sys.platform == 'win32':
    subprocess.run([sys.executable, str(base / 'build_windows.py')], check=True)
elif sys.platform == 'linux':
    build = base / 'build/linux'
    subprocess.run(['cmake', '-S', str(base), '-B', str(build), '-G', 'Ninja',
                    '-DCMAKE_BUILD_TYPE=Release'], check=True)
    subprocess.run(['cmake', '--build', str(build), '--target', 'de265', '--parallel', '4'], check=True)
else:
    raise SystemExit('Supported platforms: Windows and Linux.')
