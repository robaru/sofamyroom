import numpy as np
import pytest

import pysofamyroom as smr
from pysofamyroom import vast


def _conditions():
    seven = [125, 250, 500, 1000, 2000, 4000, 8000]
    ceiling = [0.02, 0.06, 0.14, 0.37, 0.60, 0.65, 0.65]
    floor = [0.55, 0.86, 0.83, 0.87, 0.90, 0.87, 0.87]
    absorption = np.array([ceiling] * 4 + [floor, ceiling])
    room = smr.Room((7.1, 5.1, 3.0), surface=smr.Surface(
        seven, absorption, np.full((6, 7), 0.1)))
    small = smr.Room((4.0, 3.0, 2.5), surface=smr.Surface(
        seven, absorption, np.full((6, 7), 0.1)))
    return [
        vast.RoomCondition("demo", room, (0.211 * 7.1, 0.294 * 5.1, 1.5)),
        vast.RoomCondition("small", small, (2.0, 1.5, 1.2), receiver_yaw=90),
    ]


def test_rt60_matches_matlab_port():
    room = _conditions()[0].room
    t60 = vast.rt60(room)
    assert t60.shape == (7,)
    assert np.all(t60 > 0) and np.all(np.diff(t60) <= 0)  # absorptive highs
    anechoic = smr.Room((5, 4, 3), surface=smr.Surface(
        [125, 250], np.ones((6, 2))))
    assert np.all(vast.rt60(anechoic) == 0)


def test_generate_two_rooms(sofa_path, tmp_path):
    options = smr.Options(simulatediffuse=True, numberofrays=200,
                          reflectionorder=(3, 3, 3), verbose=False)
    calls = []
    data = vast.generate(_conditions(), sofa_path, fs=16000, duration=0.25,
                         step=25, options=options, chunk=8,
                         progress=lambda *a: calls.append(a))
    n = data.n_responses
    assert n > 4 and calls[-1][1] == calls[-1][2]
    assert data.rir_left.shape == (4000, n)
    assert data.rir_left.dtype == np.float32
    assert data.hrtf_left.shape == (512, 710) and data.hrtf_fs == 44100
    assert np.abs(data.rir_left).max() > 0
    # both rooms are present, columns carry their own room size
    sizes = {tuple(np.round(c, 2)) for c in data.room_size.T}
    assert sizes == {(7.1, 5.1, 3.0), (4.0, 3.0, 2.5)}
    # sources lie inside their room, at the SOFA grid distance
    inside = (data.source_position > 0.2).all(axis=0)
    assert inside.all()
    rel = data.source_position - data.receiver_position
    assert np.allclose(np.linalg.norm(rel, axis=0), data.source_distance[0],
                       atol=1e-5)
    assert set(np.unique(data.receiver_orientation)) == {0.0, 90.0}
    for wall in vast.WALLS:
        assert data.absorption[wall].shape == (7, n)

    path = tmp_path / "demo.mat"
    data.save(path)
    back = vast.VAST.load(path)
    assert back.n_responses == n
    assert np.allclose(back.rir_left, data.rir_left)
    assert np.allclose(back.rt60_bands, data.rt60_bands, atol=1e-6)
    assert np.allclose(back.source_azimuth, data.source_azimuth)


def test_generate_rejects_empty_grid(sofa_path):
    tiny = vast.RoomCondition("tiny", smr.Room((1.0, 1.0, 1.0)), (.5, .5, .5))
    with pytest.raises(ValueError, match="no grid point"):
        vast.generate(tiny, sofa_path, duration=0.1)
