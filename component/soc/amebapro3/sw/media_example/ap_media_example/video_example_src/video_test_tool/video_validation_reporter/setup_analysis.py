"""Install an isolated PC analysis environment and build its HEVC QP helper."""
import os
from pathlib import Path
import shutil
import subprocess
import sys
import venv


def main():
    if sys.platform not in ('win32', 'linux') or not (3, 10) <= sys.version_info[:2] <= (3, 13):
        raise SystemExit('Use 64-bit Python 3.10-3.13 on Windows or Linux.')
    import struct
    if struct.calcsize('P') != 8:
        raise SystemExit('64-bit Python is required.')
    for name in ('ffmpeg', 'ffprobe'):
        if not shutil.which(name):
            raise SystemExit(f'{name} is missing from PATH; see README.md prerequisites.')
    if sys.platform == 'linux' and not shutil.which('c++'):
        raise SystemExit('Install build-essential first; see README.md.')
    base = Path(__file__).resolve().parent
    env_dir = base / ('.venv-windows' if sys.platform == 'win32' else '.venv-linux')
    venv.EnvBuilder(with_pip=True).create(env_dir)
    scripts = env_dir / ('Scripts' if sys.platform == 'win32' else 'bin')
    python = scripts / ('python.exe' if sys.platform == 'win32' else 'python')
    subprocess.run([str(python), '-m', 'pip', 'install', '--only-binary=:all:',
                    '-r', str(base / 'requirements-analysis.txt')], check=True)
    env = dict(os.environ)
    env['PATH'] = str(scripts) + os.pathsep + env.get('PATH', '')
    subprocess.run([str(python), str(base / 'hevc_qp_backend/build.py')], env=env, check=True)
    # Real known-QP tests check both decoders, not just whether imports work.
    subprocess.run([str(python), '-B', '-m', 'unittest', 'test_video_qp_integration', '-v'],
                   cwd=base, env=env, check=True)
    print(f'Analysis environment ready. Run: "{python}" "{base / "analyze_video.py"}" recording.mp4')


if __name__ == '__main__':
    try:
        main()
    except (OSError, subprocess.CalledProcessError) as error:
        raise SystemExit(f'Setup failed: {error}')
