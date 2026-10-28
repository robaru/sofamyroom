# This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
# and is licensed under the EUPL-1.2.
"""Room, options, sources and receivers: the setup of a simulation.

The classes mirror the setup file of the command line tool and the MATLAB
structure of the MEX interface one to one, with the same names and the
same defaults as ``data/sampleroomsetup.txt``. :meth:`Setup.to_text` and
:meth:`Setup.from_text` convert between the two representations.
"""

from __future__ import annotations

import warnings
from dataclasses import dataclass, field, fields
from os import PathLike
from pathlib import Path
from typing import Any

import numpy as np

__all__ = [
    "SURFACES",
    "Options",
    "Receiver",
    "Room",
    "Setup",
    "Source",
    "Surface",
    "read_setup",
]

#: Row order of :attr:`Surface.absorption` and :attr:`Surface.diffusion`:
#: the walls at x = 0 and x = Lx, the walls at y = 0 and y = Ly, the floor
#: (z = 0) and the ceiling (z = Lz).
SURFACES = ("x0", "x1", "y0", "y1", "floor", "ceiling")

_DEFAULT_FREQUENCY = (125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0)
_DEFAULT_ABSORPTION = (
    (0.10, 0.05, 0.06, 0.07, 0.10, 0.10),
    (0.14, 0.35, 0.53, 0.75, 0.70, 0.60),
    (0.10, 0.05, 0.06, 0.07, 0.10, 0.10),
    (0.10, 0.05, 0.06, 0.07, 0.10, 0.10),
    (0.01, 0.02, 0.06, 0.15, 0.25, 0.45),
    (0.24, 0.19, 0.14, 0.08, 0.13, 0.10),
)


def _vector3(value, name: str) -> tuple[float, float, float]:
    values = tuple(float(v) for v in np.asarray(value, dtype=float).ravel())
    if len(values) != 3:
        raise ValueError(f"{name} needs three values, got {values!r}")
    return values


def _matrix(value, n_bands: int, name: str) -> np.ndarray:
    array = np.array(value, dtype=float)
    if array.ndim == 1:
        array = np.tile(array, (6, 1))
    if array.shape != (6, n_bands):
        raise ValueError(
            f"{name} must have shape (6, {n_bands}) [one row per surface, "
            f"one column per frequency band], got {array.shape}")
    return array


@dataclass
class Surface:
    """Frequency-dependent absorption and diffusion of the six surfaces.

    Parameters
    ----------
    frequency : array_like
        Centre frequencies in Hz of the bands in which the coefficients
        are given. Default: 125 Hz to 4 kHz in octaves.
    absorption : array_like
        Absorption coefficients, shape ``(6, n_bands)`` in the order of
        :data:`SURFACES`. A 1-D array is used for all six surfaces.
    diffusion : array_like or None
        Scattering coefficients, same layout. ``None`` (default) uses 0.5
        everywhere.
    """

    frequency: np.ndarray = field(
        default_factory=lambda: np.array(_DEFAULT_FREQUENCY))
    absorption: np.ndarray = field(
        default_factory=lambda: np.array(_DEFAULT_ABSORPTION))
    diffusion: np.ndarray | None = None

    def __post_init__(self):
        self.frequency = np.asarray(self.frequency, dtype=float).ravel()
        n_bands = self.frequency.size
        self.absorption = _matrix(self.absorption, n_bands, "absorption")
        if self.diffusion is None:
            self.diffusion = np.full((6, n_bands), 0.5)
        self.diffusion = _matrix(self.diffusion, n_bands, "diffusion")


@dataclass
class Room:
    """Shoebox room.

    Parameters
    ----------
    dimension : array_like
        Width, depth and height ``(Lx, Ly, Lz)`` in metres.
    humidity : float
        Relative humidity, 0 to 1 (used by the ISO 9613-1 air absorption).
    temperature : float
        Temperature in degrees Celsius.
    surface : Surface
        Absorption and diffusion of the six surfaces.
    """

    dimension: tuple[float, float, float] = (10.0, 7.0, 4.0)
    humidity: float = 0.42
    temperature: float = 20.0
    surface: Surface = field(default_factory=Surface)

    def __post_init__(self):
        self.dimension = _vector3(self.dimension, "dimension")
        if isinstance(self.surface, dict):
            self.surface = Surface(**self.surface)


@dataclass
class Options:
    """Simulation options, as in the ``options`` section of a setup file.

    Parameters
    ----------
    fs : float
        Sampling frequency in Hz.
    responseduration : float
        Length of the impulse responses in seconds.
    bandsperoctave : int
        Frequency resolution of the simulation, 1 to 4 bands per octave.
    referencefrequency : float
        Reference frequency in Hz for the octave bands.
    airabsorption, distanceattenuation : bool
        Apply air absorption (ISO 9613-1) and distance attenuation.
    verbose : bool
        Print progress messages.
    simulatespecular : bool
        Simulate the specular reflections with the image-source method.
    reflectionorder : tuple of int
        Maximum reflection order along x, y and z.
    simulatediffuse : bool
        Simulate the diffuse reflections with the diffuse-rain algorithm.
    numberofrays : int
        Number of rays (rounded up to ``20 k^2``).
    diffusetimestep : float
        Time resolution in seconds of the diffuse energy histogram.
    rayenergyfloordB : float
        Energy threshold in dB below which a ray is dropped.
    uncorrelatednoise : bool
        Use uncorrelated noise for the channels of a binaural response.
    seed : int
        Seed of the random number generator; 0 selects the built-in seed
        of the original Roomsim.
    outputname : str
        Base name of the output files written by the command line tool.
    extra : dict
        Further ``options.*`` fields passed through verbatim (for example
        ``mex_saveaswav`` or fields of newer library versions).
    """

    fs: float = 44100.0
    responseduration: float = 1.25
    bandsperoctave: int = 1
    referencefrequency: float = 125.0
    airabsorption: bool = True
    distanceattenuation: bool = True
    verbose: bool = True
    simulatespecular: bool = True
    reflectionorder: tuple[int, int, int] = (10, 10, 10)
    simulatediffuse: bool = False
    numberofrays: int = 2000
    diffusetimestep: float = 0.010
    rayenergyfloordB: float = -80.0
    uncorrelatednoise: bool = True
    seed: int = 0
    outputname: str = "output"
    extra: dict[str, Any] = field(default_factory=dict)

    def __post_init__(self):
        self.reflectionorder = tuple(
            int(v) for v in _vector3(self.reflectionorder, "reflectionorder"))
        self.bandsperoctave = int(self.bandsperoctave)
        self.numberofrays = int(self.numberofrays)
        self.seed = int(self.seed)


@dataclass
class Source:
    """Sound source.

    Parameters
    ----------
    location : array_like
        Position ``(x, y, z)`` in metres.
    orientation : array_like
        Yaw, pitch and roll in degrees.
    description : str
        Directivity: ``"omnidirectional"``, ``"cardioid"``,
        ``"subcardioid"``, ``"supercardioid"``, ``"hypercardioid"``,
        ``"bidirectional"``, ``"hemisphere"``, ``"dipole"`` or
        ``"SOFA <file> [options]"``.
    """

    location: tuple[float, float, float] = (0.0, 0.0, 0.0)
    orientation: tuple[float, float, float] = (0.0, 0.0, 0.0)
    description: str = "omnidirectional"

    def __post_init__(self):
        self.location = _vector3(self.location, "location")
        self.orientation = _vector3(self.orientation, "orientation")


@dataclass
class Receiver(Source):
    """Receiver (microphone or listener).

    Same fields as :class:`Source`. A binaural receiver is described by
    ``"SOFA <file> [interp=1] [norm=1] [resampling=1] [delay=1]"``; use
    :meth:`sofa` to build that string from a path.
    """

    @classmethod
    def sofa(cls, path: str | PathLike, location, orientation=(0., 0., 0.),
             *, interp: bool = False, norm: bool = False,
             resampling: bool = False, delay: bool = False) -> Receiver:
        """Binaural receiver using the HRTFs of a SOFA file.

        Parameters
        ----------
        path : path-like
            SOFA file (SimpleFreeFieldHRIR, two receivers). Stored as an
            absolute path, quoted, so paths with spaces work.
        location, orientation : array_like
            Position in metres and yaw, pitch, roll in degrees.
        interp : bool
            Interpolate between the measured directions instead of taking
            the nearest one.
        norm : bool
            Normalise the HRTFs on loading.
        resampling : bool
            Resample the HRTFs to the simulation sampling frequency.
        delay : bool
            Apply the per-ear ``Data.Delay`` stored in the file.
        """
        resolved = Path(path).expanduser().resolve()
        flags = {"interp": interp, "norm": norm, "resampling": resampling,
                 "delay": delay}
        options = "".join(f" {k}=1" for k, v in flags.items() if v)
        return cls(location, orientation, f'SOFA "{resolved}"{options}')


@dataclass
class Setup:
    """Complete description of one simulation run.

    Parameters
    ----------
    room : Room
    options : Options
    sources : list of Source
    receivers : list of Receiver
        A response is simulated for every source/receiver pair.
    """

    room: Room = field(default_factory=Room)
    options: Options = field(default_factory=Options)
    sources: list[Source] = field(default_factory=list)
    receivers: list[Receiver] = field(default_factory=list)

    def __post_init__(self):
        if isinstance(self.room, dict):
            self.room = Room(**self.room)
        if isinstance(self.options, dict):
            self.options = Options(**self.options)
        self.sources = [s if isinstance(s, Source) else Source(**s)
                        for s in self.sources]
        self.receivers = [r if isinstance(r, Receiver) else Receiver(**r)
                          for r in self.receivers]

    # -- text form -----------------------------------------------------------

    def to_text(self) -> str:
        """Setup file text understood by the command line tool."""
        room, surface, options = self.room, self.room.surface, self.options
        lines = [
            f"room.dimension = {_fmt(room.dimension)};",
            f"room.humidity = {_fmt(room.humidity)};",
            f"room.temperature = {_fmt(room.temperature)};",
            f"room.surface.frequency = {_fmt(surface.frequency)};",
            f"room.surface.absorption = {_fmt(surface.absorption)};",
            f"room.surface.diffusion = {_fmt(surface.diffusion)};",
        ]
        for f in fields(Options):
            if f.name == "extra":
                continue
            value = getattr(options, f.name)
            lines.append(f"options.{f.name} = {_fmt(value)};")
        for name, value in options.extra.items():
            lines.append(f"options.{name} = {_fmt(value)};")
        if not self.sources or not self.receivers:
            raise ValueError("a setup needs at least one source and one "
                             "receiver")
        for kind, items in (("source", self.sources),
                            ("receiver", self.receivers)):
            for i, item in enumerate(items, start=1):
                lines.append(f"{kind}({i}).location = {_fmt(item.location)};")
                lines.append(
                    f"{kind}({i}).orientation = {_fmt(item.orientation)};")
                lines.append(
                    f"{kind}({i}).description = {_fmt(item.description)};")
        return "\n".join(lines) + "\n"

    @classmethod
    def from_text(cls, text: str) -> Setup:
        """Parse the text of a setup file (comments allowed)."""
        tree: dict[str, Any] = {}
        for key, raw in _statements(text):
            _assign(tree, _split_key(key), _convert(raw))

        known_options = {f.name for f in fields(Options)} - {"extra"}
        opt = dict(tree.get("options", {}))
        extra = {k: opt.pop(k) for k in list(opt) if k not in known_options}
        room = dict(tree.get("room", {}))
        for name in ("source", "receiver"):
            if name in tree and not isinstance(tree[name], dict):
                raise ValueError(f"'{name}' must be indexed: {name}(1)...")
        return cls(
            room=Room(**room),
            options=Options(**opt, extra=extra),
            sources=[Source(**v) for _, v in sorted(
                tree.get("source", {}).items())],
            receivers=[Receiver(**v) for _, v in sorted(
                tree.get("receiver", {}).items())],
        )

    @classmethod
    def from_file(cls, path: str | PathLike) -> Setup:
        """Read a setup file."""
        return cls.from_text(Path(path).read_text())

    def save(self, path: str | PathLike) -> None:
        """Write the setup file."""
        Path(path).write_text(self.to_text())

    # -- running -------------------------------------------------------------

    def simulate(self) -> list[list[np.ndarray]]:
        """Run the simulation, see :func:`pysofamyroom.simulate`."""
        from pysofamyroom._native import simulate_text
        return simulate_text(self.to_text())


def read_setup(path: str | PathLike) -> Setup:
    """Read a setup file (same as :meth:`Setup.from_file`)."""
    return Setup.from_file(path)


# -- formatting --------------------------------------------------------------

def _fmt_number(x) -> str:
    x = float(x)
    if x.is_integer() and abs(x) < 1e15:
        return str(int(x))
    return repr(x)


def _fmt(value) -> str:
    if isinstance(value, (bool, np.bool_)):
        return "true" if value else "false"
    if isinstance(value, str):
        if "'" in value:
            raise ValueError(f"single quotes are not allowed in {value!r}")
        return f"'{value}'"
    array = np.asarray(value)
    if array.ndim == 0:
        return _fmt_number(array)
    if array.ndim == 1:
        return "[ " + " ".join(_fmt_number(v) for v in array) + " ]"
    if array.ndim == 2:
        rows = ["  ".join(_fmt_number(v) for v in row) for row in array]
        return "[ " + "; ".join(rows) + " ]"
    raise ValueError(f"cannot write a {array.ndim}-D array to a setup file")


# -- parsing -----------------------------------------------------------------

def _statements(text: str):
    """Yield ``(key, raw_value)`` pairs; ``%`` comments are dropped."""
    buf: list[str] = []
    in_quote = False
    depth = 0
    i, n = 0, len(text)
    while i < n:
        ch = text[i]
        if in_quote:
            buf.append(ch)
            if ch == "'":
                in_quote = False
        elif ch == "%":
            while i < n and text[i] != "\n":
                i += 1
            continue
        elif ch == "'":
            in_quote = True
            buf.append(ch)
        elif ch == "[":
            depth += 1
            buf.append(ch)
        elif ch == "]":
            depth -= 1
            buf.append(ch)
        elif ch == ";" and depth == 0:
            yield _split_statement("".join(buf))
            buf = []
        elif ch == "\n" and depth == 0 and not in_quote and "=" in buf:
            # a statement without the trailing semicolon
            yield _split_statement("".join(buf))
            buf = []
        else:
            buf.append(ch)
        i += 1
    if "".join(buf).strip():
        yield _split_statement("".join(buf))


def _split_statement(statement: str) -> tuple[str, str]:
    if "=" not in statement:
        raise ValueError(f"cannot parse setup line {statement.strip()!r}")
    key, raw = statement.split("=", 1)
    return key.strip(), raw.strip()


def _split_key(key: str) -> list:
    parts: list = []
    for token in key.split("."):
        if "(" in token:
            name, index = token[:-1].split("(")
            parts += [name, int(index)]
        else:
            parts.append(token)
    return parts


def _assign(tree: dict, parts: list, value) -> None:
    node = tree
    for part in parts[:-1]:
        node = node.setdefault(part, {})
    if parts[-1] in node:
        warnings.warn(f"duplicate setup field {'.'.join(map(str, parts))}",
                      stacklevel=3)
    node[parts[-1]] = value


def _convert(raw: str):
    if raw.startswith("["):
        body = raw[1:raw.rindex("]")]
        rows = [r for r in body.split(";") if r.strip()]
        values = [[float(v) for v in r.replace(",", " ").split()]
                  for r in rows]
        array = np.array(values, dtype=float)
        return array[0] if len(values) == 1 else array
    if raw.startswith("'"):
        return raw[1:raw.rindex("'")]
    if raw.lower() == "true":
        return True
    if raw.lower() == "false":
        return False
    return float(raw)
