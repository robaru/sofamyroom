"""Python version of matlab/VAST/DemoVAST.m.

Builds a small VAST dataset: the room of the VAST paper in two sizes, the
MIT KEMAR HRTFs, responses stored at 16 kHz. The SOFA file is downloaded
if it is not in data/. Run from the repository root:

    python examples/vast_demo.py [output.mat]
"""

import sys
import urllib.request
from pathlib import Path

import numpy as np

import pysofamyroom as smr
from pysofamyroom import vast

SOFA_URL = ("https://sofacoustics.org/data/database/mit/"
            "mit_kemar_normal_pinna.sofa")


def main(output="vast_demo.mat"):
    """Generate the demo dataset and write it to ``output``."""
    sofa = Path("data/MIT_KEMAR_normal_pinna.sofa")
    if not sofa.is_file():
        sofa.parent.mkdir(exist_ok=True)
        print(f"downloading {SOFA_URL} ...")
        urllib.request.urlretrieve(SOFA_URL, sofa)

    # acoustic parameters of DemoVAST.m: 7 octave bands, 125 Hz to 8 kHz
    frequency = [125, 250, 500, 1000, 2000, 4000, 8000]
    ceiling = [0.02, 0.06, 0.14, 0.37, 0.60, 0.65, 0.65]
    floor = [0.55, 0.86, 0.83, 0.87, 0.90, 0.87, 0.87]
    walls = ceiling
    absorption = np.array([walls] * 4 + [floor, ceiling])
    diffusion = 0.1

    def room(size):
        return smr.Room(size, surface=smr.Surface(
            frequency, absorption, np.full((6, 7), diffusion)))

    conditions = [
        vast.RoomCondition("vast_room", room((7.1, 5.1, 3.0)),
                           receiver_location=(0.211 * 7.1, 0.294 * 5.1, 1.5)),
        vast.RoomCondition("vast_room_small", room((4.5, 3.5, 2.6)),
                           receiver_location=(1.5, 1.2, 1.5),
                           receiver_yaw=45.0),
    ]

    def progress(name, done, total):
        print(f"{name}: {done}/{total} responses", flush=True)

    dataset = vast.generate(conditions, sofa, fs=16000, duration=1.25,
                            progress=progress)
    dataset.save(output)
    print(f"{dataset.n_responses} responses written to {output}")
    print("RT60 per band (s):", np.round(dataset.rt60_bands[:, 0], 2))


if __name__ == "__main__":
    main(*sys.argv[1:])
