# This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
# and is licensed under the EUPL-1.2.
"""Python interface to SofaMyRoom.

SofaMyRoom is a "shoebox" room acoustics simulator that renders binaural
room impulse responses (BRIRs) with head-related transfer functions stored
in SOFA files. This package drives the C library in-process and uses the
same parameters, with the same names and defaults, as the setup files of
the command line tool and the MATLAB structure of the MEX interface.

Example
-------
>>> import pysofamyroom as smr
>>> setup = smr.Setup(
...     room=smr.Room(dimension=(10, 7, 4)),
...     sources=[smr.Source((8, 2.5, 1.6), (180, 0, 0), "subcardioid")],
...     receivers=[smr.Receiver.sofa("kemar.sofa", (3, 5, 1.2))],
... )
>>> brirs = smr.simulate(setup)
>>> brirs[0][0].shape          # (n_samples, n_channels)
(55125, 2)
"""

from __future__ import annotations

from os import PathLike
from pathlib import Path

import numpy as np

from pysofamyroom._native import (
    SofaMyRoomError,
    clear_sensors,
    library_version,
    set_log,
    simulate_text,
)
from pysofamyroom.roomsetup import (
    SURFACES,
    Options,
    Receiver,
    Room,
    Setup,
    Source,
    Surface,
    read_setup,
)

__version__ = "1.0.0"

__all__ = [
    "SURFACES",
    "Options",
    "Receiver",
    "Room",
    "Setup",
    "SofaMyRoomError",
    "Source",
    "Surface",
    "__version__",
    "clear_sensors",
    "library_version",
    "read_setup",
    "set_log",
    "simulate",
]


def simulate(setup: Setup | str | PathLike) -> list[list[np.ndarray]]:
    """Simulate the room impulse responses of a setup.

    Parameters
    ----------
    setup : Setup, path-like or str
        A :class:`Setup`, the path of a setup file, or the text of one.

    Returns
    -------
    list of list of numpy.ndarray
        ``brirs[s][r]`` is the response of source ``s`` at receiver ``r``,
        shape ``(n_samples, n_channels)``, float64, sampled at
        ``setup.options.fs``. Same layout as the MEX cell array.

    Raises
    ------
    SofaMyRoomError
        If the setup is invalid or the simulation fails.

    Notes
    -----
    The library is single-threaded and keeps global state (a cache of the
    loaded SOFA files), so calls are serialised with a lock. Use processes,
    not threads, to simulate several rooms in parallel.
    """
    if isinstance(setup, Setup):
        text = setup.to_text()
    elif isinstance(setup, PathLike) or (
            isinstance(setup, str) and "=" not in setup):
        text = Path(setup).read_text()
    else:
        text = str(setup)
    return simulate_text(text)
