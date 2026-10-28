# This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
# and is licensed under the EUPL-1.2.
"""Python interface to SofaMyRoom.

SofaMyRoom is a "shoebox" room acoustics simulator that renders binaural
room impulse responses (BRIRs) with head-related transfer functions stored
in SOFA files. This package drives the C library in-process and uses the
same parameters, with the same names and defaults, as the setup files of
the command line tool and the MATLAB structure of the MEX interface.
"""

__version__ = "1.0.0"

from pysofamyroom._native import (
    SofaMyRoomError,
    clear_sensors,
    library_version,
    set_log,
)

__all__ = [
    "SofaMyRoomError",
    "__version__",
    "clear_sensors",
    "library_version",
    "set_log",
]
