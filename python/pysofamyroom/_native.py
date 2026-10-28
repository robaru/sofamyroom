# This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
# and is licensed under the EUPL-1.2.
"""ctypes binding to the SofaMyRoom shared library.

The C side is ``src/libroomsim/include/capi.h``: a setup is passed as the
text of a setup file and the responses come back as flat double arrays,
so nothing but scalars, strings and pointers crosses the boundary.
"""

from __future__ import annotations

import ctypes
import os
import sys
import sysconfig
import threading
from collections.abc import Callable
from pathlib import Path

import numpy as np

__all__ = [
    "SofaMyRoomError",
    "clear_sensors",
    "library_version",
    "set_log",
    "simulate_text",
]


class SofaMyRoomError(RuntimeError):
    """Raised when the simulator rejects a setup or fails while running."""


class _Brir(ctypes.Structure):
    _fields_ = [
        ("fs", ctypes.c_double),
        ("n_samples", ctypes.c_int),
        ("n_channels", ctypes.c_int),
        ("sample", ctypes.POINTER(ctypes.c_double)),
    ]


class _Result(ctypes.Structure):
    _fields_ = [
        ("n_sources", ctypes.c_int),
        ("n_receivers", ctypes.c_int),
        ("brir", ctypes.POINTER(_Brir)),
        ("handle", ctypes.c_void_p),
    ]


_LogFn = ctypes.CFUNCTYPE(None, ctypes.c_char_p)

_LIBRARY_NAMES = (
    "libpysofamyroom_c.dylib",
    "libpysofamyroom_c.so",
    "pysofamyroom_c.dll",
    "libpysofamyroom_c.dll",
)

# one simulation at a time: the C library keeps global state
_lock = threading.Lock()
_lib: ctypes.CDLL | None = None
_log_callback = None  # keeps the CFUNCTYPE object alive


def _candidate_paths():
    env = os.environ.get("PYSOFAMYROOM_LIBRARY")
    if env:
        path = Path(env)
        if path.is_file():
            yield path
        else:
            yield from (path / name for name in _LIBRARY_NAMES)
    here = Path(__file__).resolve().parent
    yield from (here / name for name in _LIBRARY_NAMES)
    # editable installs keep the compiled library in site-packages
    for key in ("platlib", "purelib"):
        folder = Path(sysconfig.get_paths()[key]) / "pysofamyroom"
        yield from (folder / name for name in _LIBRARY_NAMES)


def _find_library() -> Path:
    for path in _candidate_paths():
        if path.is_file():
            return path
    raise ImportError(
        "the SofaMyRoom shared library was not found; reinstall "
        "pysofamyroom or point PYSOFAMYROOM_LIBRARY at the file")


def _default_log(text: bytes) -> None:
    sys.stdout.write(text.decode(errors="replace"))
    sys.stdout.flush()


def _load() -> ctypes.CDLL:
    global _lib
    if _lib is not None:
        return _lib
    lib = ctypes.CDLL(str(_find_library()))
    lib.smr_version.restype = ctypes.c_char_p
    lib.smr_version.argtypes = []
    lib.smr_simulate_text.restype = ctypes.c_int
    lib.smr_simulate_text.argtypes = [
        ctypes.c_char_p,
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.POINTER(_Result)),
    ]
    lib.smr_result_free.restype = None
    lib.smr_result_free.argtypes = [ctypes.POINTER(_Result)]
    lib.smr_last_error.restype = ctypes.c_char_p
    lib.smr_last_error.argtypes = []
    lib.smr_clear_sensors.restype = None
    lib.smr_clear_sensors.argtypes = []
    lib.smr_set_log.restype = None
    lib.smr_set_log.argtypes = [_LogFn]
    _lib = lib
    # route the simulator's messages through Python's stdout (notebooks,
    # pytest capture) instead of the C stdout
    set_log(_default_log)
    return lib


def library_version() -> str:
    """Version string of the underlying SofaMyRoom C library."""
    return _load().smr_version().decode()


def clear_sensors() -> None:
    """Drop the sources and receivers (SOFA files) cached between calls."""
    with _lock:
        _load().smr_clear_sensors()


def set_log(callback: Callable[[bytes], None] | None) -> None:
    """Choose where the simulator's status messages go.

    Parameters
    ----------
    callback : callable or None
        Called with each message as ``bytes``. ``None`` sends the messages
        to the C standard output. The default writes to ``sys.stdout``.
    """
    global _log_callback
    lib = _load()
    if callback is None:
        _log_callback = ctypes.cast(None, _LogFn)
    else:
        _log_callback = _LogFn(callback)
    lib.smr_set_log(_log_callback)


def simulate_text(text: str) -> list[list[np.ndarray]]:
    """Run a simulation from the text of a setup file.

    Parameters
    ----------
    text : str
        Setup in the grammar of the command line tool (see
        ``data/sampleroomsetup.txt``). Relative paths, for example to SOFA
        files, are resolved against the current working directory.

    Returns
    -------
    list of list of numpy.ndarray
        ``brirs[s][r]`` is the response of source ``s`` at receiver ``r``,
        shape ``(n_samples, n_channels)``, float64, like the array returned
        by the MEX interface.

    Raises
    ------
    SofaMyRoomError
        If the setup is invalid or the simulation fails.
    """
    lib = _load()
    raw = text.encode()
    out = ctypes.POINTER(_Result)()
    with _lock:
        code = lib.smr_simulate_text(raw, len(raw), ctypes.byref(out))
        if code != 0:
            message = lib.smr_last_error().decode(errors="replace")
            raise SofaMyRoomError(message or f"simulation failed ({code})")
        try:
            result = out.contents
            n_sources, n_receivers = result.n_sources, result.n_receivers
            brirs = [[None] * n_receivers for _ in range(n_sources)]
            for r in range(n_receivers):
                for s in range(n_sources):
                    item = result.brir[r * n_sources + s]
                    data = np.ctypeslib.as_array(
                        item.sample, shape=(item.n_channels, item.n_samples))
                    brirs[s][r] = np.ascontiguousarray(data.T)
        finally:
            lib.smr_result_free(out)
    return brirs
