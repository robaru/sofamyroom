# This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
# and is licensed under the EUPL-1.2.
"""Generate annotated BRIR datasets in the VAST format.

VAST (Virtual Acoustic Space Traveler, Gaultier et al. 2017) stores
binaural room impulse responses together with the room, receiver and
source parameters that produced them. This module is a port of
``matlab/VAST/VASTGeneration.m``: for each :class:`RoomCondition` the
source positions are the measurement grid of the SOFA file, shifted to the
receiver and clipped to the room, one BRIR is simulated per position, and
all conditions end up in one :class:`VAST` object that can be saved as a
MATLAB-compatible ``.mat`` file with the layout of the original dataset.

Example
-------
>>> import pysofamyroom as smr
>>> from pysofamyroom import vast
>>> conditions = [
...     vast.RoomCondition("small", smr.Room((4.5, 3.5, 2.6)),
...                        receiver_location=(1.2, 1.7, 1.5)),
...     vast.RoomCondition("large", smr.Room((9.0, 7.0, 3.5)),
...                        receiver_location=(3.0, 2.5, 1.5)),
... ]
>>> dataset = vast.generate(conditions, "kemar.sofa", fs=16000)
>>> dataset.save("vast_kemar.mat")
"""

from __future__ import annotations

from collections.abc import Callable, Sequence
from dataclasses import dataclass, field
from fractions import Fraction
from os import PathLike
from pathlib import Path

import numpy as np
import scipy.io
import scipy.signal

from pysofamyroom.roomsetup import Options, Receiver, Room, Setup, Source

__all__ = [
    "VAST",
    "RoomCondition",
    "WALLS",
    "generate",
    "rt60",
]

#: VAST wall names in the row order of :data:`pysofamyroom.SURFACES`
#: (x = 0, x = Lx, y = 0, y = Ly, floor, ceiling).
WALLS = ("WestWall", "EastWall", "SouthWall", "NorthWall", "Floor",
         "Ceiling")


@dataclass
class RoomCondition:
    """One acoustic condition: a room and a receiver inside it.

    Parameters
    ----------
    name : str
        Label of the condition (not stored in the ``.mat`` file).
    room : Room
        Room dimensions, absorption, diffusion, humidity and temperature.
    receiver_location : array_like
        Receiver position ``(x, y, z)`` in metres.
    receiver_yaw : float
        Receiver orientation in degrees around the vertical axis, 0 facing
        +x. The source grid turns with the receiver, so the direction of
        every source relative to the head is the one of the SOFA grid.
    """

    name: str
    room: Room = field(default_factory=Room)
    receiver_location: tuple[float, float, float] = (1.0, 1.0, 1.5)
    receiver_yaw: float = 0.0


@dataclass
class VAST:
    """A VAST dataset in memory.

    Responses are stored as columns, ``n`` is the number of responses,
    ``n_bands`` the number of frequency bands. The attribute names follow
    the MATLAB structure of ``vast.rst``.
    """

    fs: float
    frequency: np.ndarray            # (n_bands,)
    duration: float
    rir_left: np.ndarray             # (length, n) float32
    rir_right: np.ndarray            # (length, n) float32
    room_size: np.ndarray            # (3, n)
    rt60_bands: np.ndarray           # (n_bands, n)
    rt60_global: np.ndarray          # (1, n)
    absorption: dict[str, np.ndarray]  # wall -> (n_bands, n)
    diffusion: np.ndarray            # (n_bands, n)
    receiver_position: np.ndarray    # (3, n)
    receiver_orientation: np.ndarray  # (1, n)
    hrtf_fs: float
    hrtf_left: np.ndarray            # (hrir_length, n_directions)
    hrtf_right: np.ndarray
    source_azimuth: np.ndarray       # (1, n) degrees
    source_elevation: np.ndarray     # (1, n) degrees
    source_distance: np.ndarray      # (1, n) metres
    source_position: np.ndarray      # (3, n) metres, room coordinates

    @property
    def n_responses(self) -> int:
        """Number of stored responses."""
        return self.rir_left.shape[1]

    def extend(self, other: VAST) -> VAST:
        """Append the responses of ``other`` (same fs, bands and HRTF)."""
        if (other.fs != self.fs or other.duration != self.duration
                or not np.array_equal(other.frequency, self.frequency)
                or other.hrtf_left.shape != self.hrtf_left.shape):
            raise ValueError("datasets to merge must share fs, duration, "
                             "frequency bands and HRTF set")
        for name in ("rir_left", "rir_right", "room_size", "rt60_bands",
                     "rt60_global", "diffusion", "receiver_position",
                     "receiver_orientation", "source_azimuth",
                     "source_elevation", "source_distance",
                     "source_position"):
            setattr(self, name, np.hstack(
                [getattr(self, name), getattr(other, name)]))
        self.absorption = {w: np.hstack([self.absorption[w],
                                         other.absorption[w]])
                           for w in WALLS}
        return self

    def to_dict(self) -> dict:
        """Nested dictionary with the MATLAB field names of VAST."""
        single = np.float32
        return {
            "GlobalParams": {
                "SamplingFrequency": float(self.fs),
                "FreqBin": np.asarray(self.frequency, dtype=float),
                "RIRLengthTime": float(self.duration),
                "RIRLengthSamples": float(self.rir_left.shape[0]),
            },
            "RIR": {
                "Left": self.rir_left.astype(single),
                "Right": self.rir_right.astype(single),
            },
            "Room": {
                "Size": self.room_size.astype(single),
                "FreqRT60": self.rt60_bands.astype(single),
                "GlobalRT60": self.rt60_global.astype(single),
                "Absorption": {w: self.absorption[w].astype(single)
                               for w in WALLS},
                "Diffusion": self.diffusion.astype(single),
            },
            "Receiver": {
                "Position": self.receiver_position.astype(single),
                "Orientation": self.receiver_orientation.astype(single),
                "HRTF": {
                    "Fs": float(self.hrtf_fs),
                    "Left": np.asarray(self.hrtf_left, dtype=float),
                    "Right": np.asarray(self.hrtf_right, dtype=float),
                },
            },
            "Source": {
                "Azimuth": self.source_azimuth.astype(single),
                "Elevation": self.source_elevation.astype(single),
                "Distance": self.source_distance.astype(single),
                "AbsolutePos": self.source_position.astype(single),
            },
        }

    def save(self, path: str | PathLike) -> None:
        """Write a ``.mat`` file holding the structure ``VAST``."""
        scipy.io.savemat(Path(path), {"VAST": self.to_dict()},
                         do_compression=True, oned_as="row")

    @classmethod
    def load(cls, path: str | PathLike) -> VAST:
        """Read a ``.mat`` file written by :meth:`save` (or by MATLAB)."""
        d = scipy.io.loadmat(Path(path), simplify_cells=True)["VAST"]
        two_d = np.atleast_2d
        return cls(
            fs=float(d["GlobalParams"]["SamplingFrequency"]),
            frequency=np.atleast_1d(d["GlobalParams"]["FreqBin"]).astype(float),
            duration=float(d["GlobalParams"]["RIRLengthTime"]),
            rir_left=two_d(d["RIR"]["Left"]),
            rir_right=two_d(d["RIR"]["Right"]),
            room_size=two_d(d["Room"]["Size"]),
            rt60_bands=two_d(d["Room"]["FreqRT60"]),
            rt60_global=two_d(d["Room"]["GlobalRT60"]),
            absorption={w: two_d(d["Room"]["Absorption"][w]) for w in WALLS},
            diffusion=two_d(d["Room"]["Diffusion"]),
            receiver_position=two_d(d["Receiver"]["Position"]),
            receiver_orientation=two_d(d["Receiver"]["Orientation"]),
            hrtf_fs=float(d["Receiver"]["HRTF"]["Fs"]),
            hrtf_left=two_d(d["Receiver"]["HRTF"]["Left"]),
            hrtf_right=two_d(d["Receiver"]["HRTF"]["Right"]),
            source_azimuth=two_d(d["Source"]["Azimuth"]),
            source_elevation=two_d(d["Source"]["Elevation"]),
            source_distance=two_d(d["Source"]["Distance"]),
            source_position=two_d(d["Source"]["AbsolutePos"]),
        )


def rt60(room: Room) -> np.ndarray:
    """Norris-Eyring reverberation time per frequency band.

    Port of ``Room2RT60`` in ``VASTGeneration.m`` (air absorption after
    Roomsim's ``estimateRT60.m``). Returns zeros for an anechoic room.

    Parameters
    ----------
    room : Room
        Room dimensions, absorption per surface and band, humidity and
        temperature.

    Returns
    -------
    numpy.ndarray
        RT60 in seconds, one value per band of ``room.surface.frequency``.
    """
    lx, ly, lz = room.dimension
    f = room.surface.frequency
    alpha = room.surface.absorption            # (6, n_bands)

    air = 5.5e-4 * (0.50 / room.humidity) * (f / 1000.0) ** 1.7
    c = np.round(331.0 * np.sqrt(1.0 + 0.0036 * room.temperature))
    volume = lx * ly * lz
    a_xy, a_xz, a_yz = lx * ly, lx * lz, ly * lz
    surface = 2.0 * (a_xy + a_xz + a_yz)
    effective = (a_yz * (alpha[0] + alpha[1]) + a_xz * (alpha[2] + alpha[3])
                 + a_xy * (alpha[4] + alpha[5]))
    mean_alpha = effective / surface

    eps = np.finfo(float).eps
    result = np.zeros_like(f)
    live = np.abs(1.0 - mean_alpha) >= eps
    result[live] = (55.25 / c) * volume / (
        4.0 * air[live] * volume
        - surface * np.log(1.0 - mean_alpha[live] + eps))
    return result


def _read_sofa(path: Path):
    """Source grid (cartesian and spherical) and HRIRs of a SOFA file."""
    import sofar

    sofa = sofar.read_sofa(path, verify=False)
    pos = np.asarray(sofa.SourcePosition, dtype=float)
    kind = str(getattr(sofa, "SourcePosition_Type", "spherical")).lower()
    if kind.startswith("spherical"):
        az, el, r = np.radians(pos[:, 0]), np.radians(pos[:, 1]), pos[:, 2]
        cart = np.column_stack([r * np.cos(el) * np.cos(az),
                                r * np.cos(el) * np.sin(az),
                                r * np.sin(el)])
        sph = pos.copy()
    else:
        cart = pos.copy()
        r = np.linalg.norm(cart, axis=1)
        az = np.degrees(np.arctan2(cart[:, 1], cart[:, 0])) % 360.0
        el = np.degrees(np.arcsin(np.divide(
            cart[:, 2], r, out=np.zeros_like(r), where=r > 0)))
        sph = np.column_stack([az, el, r])
    ir = np.asarray(sofa.Data_IR, dtype=float)          # (M, R, N)
    if ir.ndim != 3 or ir.shape[1] != 2:
        raise ValueError("the SOFA file must hold two-receiver HRIRs "
                         f"(Data.IR of shape (M, 2, N)), got {ir.shape}")
    fs = float(sofa.Data_SamplingRate)
    return cart, sph, ir[:, 0, :].T, ir[:, 1, :].T, fs


def _rotate_z(points: np.ndarray, yaw_deg: float) -> np.ndarray:
    a = np.radians(yaw_deg)
    rot = np.array([[np.cos(a), -np.sin(a), 0.0],
                    [np.sin(a), np.cos(a), 0.0],
                    [0.0, 0.0, 1.0]])
    return points @ rot.T


def generate(conditions: RoomCondition | Sequence[RoomCondition],
             sofa: str | PathLike, *, fs: float = 16000.0,
             duration: float = 1.25, margin: float = 0.2, step: int = 1,
             options: Options | None = None, interp: bool = True,
             norm: bool = True, resampling: bool = True,
             chunk: int = 32, verbose: bool = False,
             progress: Callable[[str, int, int], None] | None = None,
             ) -> VAST:
    """Simulate a VAST dataset for one or more room conditions.

    Parameters
    ----------
    conditions : RoomCondition or sequence of RoomCondition
        Rooms and receivers; the responses of all conditions are stacked
        in one dataset.
    sofa : path-like
        SOFA file with the HRTFs of the listener. Its measurement grid,
        centred on the receiver, gives the source positions.
    fs : float
        Sampling frequency of the stored responses. The simulation runs
        at the sampling frequency of the HRTFs and the result is
        resampled, as in the MATLAB generator.
    duration : float
        Length of the responses in seconds.
    margin : float
        Grid points closer than this (metres) to a wall are skipped.
    step : int
        Use every ``step``-th grid point (1 = all points).
    options : Options or None
        Simulation options; ``fs`` and ``responseduration`` are
        overridden. Default: the VAST settings (bands per octave 1,
        specular order 10, 2000 rays, diffuse rain on, uncorrelated noise).
    interp, norm, resampling : bool
        Flags of the SOFA receiver (see :meth:`Receiver.sofa`).
    chunk : int
        Number of sources per call of the simulator (bounds memory use).
    verbose : bool
        Print the simulator's messages.
    progress : callable or None
        Called as ``progress(condition_name, done, total)`` after each
        chunk.

    Returns
    -------
    VAST
    """
    if isinstance(conditions, RoomCondition):
        conditions = [conditions]
    if not conditions:
        raise ValueError("no room condition given")
    sofa = Path(sofa)
    grid_cart, grid_sph, hrtf_left, hrtf_right, hrtf_fs = _read_sofa(sofa)
    grid_cart, grid_sph = grid_cart[::step], grid_sph[::step]

    base = options if options is not None else Options(
        simulatediffuse=True, verbose=verbose)
    frequency = conditions[0].room.surface.frequency
    n_bands = frequency.size
    length = int(round(duration * fs))
    up_down = Fraction(fs / hrtf_fs).limit_denominator(10000)

    parts: list[VAST] = []
    for cond in conditions:
        room = cond.room
        if room.surface.frequency.size != n_bands or not np.allclose(
                room.surface.frequency, frequency):
            raise ValueError("all conditions must use the same frequency "
                             "bands")
        receiver = np.asarray(cond.receiver_location, dtype=float)
        cart = _rotate_z(grid_cart, cond.receiver_yaw) + receiver
        lo, hi = margin, np.asarray(room.dimension) - margin
        valid = np.all((cart > lo) & (cart < hi), axis=1)
        positions, spherical = cart[valid], grid_sph[valid]
        n = len(positions)
        if n == 0:
            raise ValueError(f"condition {cond.name!r}: no grid point of "
                             f"{sofa.name} falls inside the room")

        opts = Options(**{**vars(base), "fs": hrtf_fs,
                          "responseduration": duration})
        rec = Receiver.sofa(sofa, receiver, (cond.receiver_yaw, 0.0, 0.0),
                            interp=interp, norm=norm, resampling=resampling)
        left = np.empty((length, n), dtype=np.float32)
        right = np.empty((length, n), dtype=np.float32)
        for start in range(0, n, chunk):
            stop = min(start + chunk, n)
            setup = Setup(room=room, options=opts,
                          sources=[Source(p) for p in positions[start:stop]],
                          receivers=[rec])
            for i, brir in enumerate(setup.simulate()):
                ir = brir[0]
                if up_down != 1:
                    ir = scipy.signal.resample_poly(
                        ir, up_down.numerator, up_down.denominator, axis=0)
                ir = ir[:length]
                left[:len(ir), start + i] = ir[:, 0]
                right[:len(ir), start + i] = ir[:, 1]
                left[len(ir):, start + i] = 0.0
                right[len(ir):, start + i] = 0.0
            if progress is not None:
                progress(cond.name, stop, n)

        t60 = rt60(room)
        rows = room.surface.absorption
        parts.append(VAST(
            fs=fs, frequency=frequency.copy(), duration=duration,
            rir_left=left, rir_right=right,
            room_size=np.tile(np.asarray(room.dimension)[:, None], (1, n)),
            rt60_bands=np.tile(t60[:, None], (1, n)),
            rt60_global=np.full((1, n), t60.mean()),
            absorption={w: np.tile(rows[k][:, None], (1, n))
                        for k, w in enumerate(WALLS)},
            diffusion=np.tile(room.surface.diffusion.mean(axis=0)[:, None],
                              (1, n)),
            receiver_position=np.tile(receiver[:, None], (1, n)),
            receiver_orientation=np.full((1, n), float(cond.receiver_yaw)),
            hrtf_fs=hrtf_fs, hrtf_left=hrtf_left, hrtf_right=hrtf_right,
            source_azimuth=spherical[:, 0][None, :],
            source_elevation=spherical[:, 1][None, :],
            source_distance=spherical[:, 2][None, :],
            source_position=positions.T.copy(),
        ))

    dataset = parts[0]
    for part in parts[1:]:
        dataset.extend(part)
    return dataset
