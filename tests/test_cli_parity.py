"""The Python package must produce exactly what the command line tool does.

The test runs only when a built ``sofamyroom`` executable is found (the
CMake build directory, the scikit-build-core build directory, or the path
in the SOFAMYROOM_CLI environment variable).
"""

import os
import shutil
import subprocess
from pathlib import Path

import numpy as np
import pytest
import scipy.io.wavfile

import pysofamyroom as smr

REPO = Path(__file__).resolve().parents[1]


def _find_cli() -> Path | None:
    env = os.environ.get("SOFAMYROOM_CLI")
    if env and Path(env).is_file():
        return Path(env)
    candidates = list((REPO / "build").glob("**/sofamyroom"))
    candidates += list((REPO / "build").glob("**/sofamyroom.exe"))
    candidates += list((REPO / "build").glob("**/Release/sofamyroom.exe"))
    for path in candidates:
        if path.is_file() and os.access(path, os.X_OK):
            return path
    return shutil.which("sofamyroom") and Path(shutil.which("sofamyroom"))


@pytest.mark.skipif(_find_cli() is None, reason="sofamyroom CLI not built")
def test_python_matches_cli_bit_for_bit(sample_setup_text, tmp_path):
    cli = _find_cli()
    setup_file = tmp_path / "setup.txt"
    setup_file.write_text(sample_setup_text)
    subprocess.run([str(cli), str(setup_file)], cwd=tmp_path, check=True,
                   capture_output=True)
    fs, wav = scipy.io.wavfile.read(tmp_path / "output_receiver_0.wav")
    assert fs == 44100 and wav.dtype == np.float32

    brir = smr.simulate(sample_setup_text)[0][0]
    assert brir.shape == wav.shape
    # the CLI writes float32 samples; the library computes in float64
    assert np.array_equal(brir.astype(np.float32), wav)
