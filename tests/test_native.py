import re

import numpy as np
import pytest

import pysofamyroom
from pysofamyroom._native import simulate_text


def test_versions():
    assert re.match(r"\d+\.\d+\.\d+", pysofamyroom.__version__)
    assert pysofamyroom.library_version().startswith("1.")


def test_simulate_text_shape(sample_setup_text):
    brirs = simulate_text(sample_setup_text)
    assert len(brirs) == 1 and len(brirs[0]) == 1
    brir = brirs[0][0]
    assert brir.shape == (int(1.25 * 44100), 2)
    assert brir.dtype == np.float64
    assert brir.flags.c_contiguous
    assert np.abs(brir).max() > 0


def test_simulate_text_is_deterministic(sample_setup_text):
    a = simulate_text(sample_setup_text)[0][0]
    b = simulate_text(sample_setup_text)[0][0]
    assert np.array_equal(a, b)


def test_errors_are_raised_and_recoverable(sample_setup_text):
    missing_field = sample_setup_text.replace("options.fs ", "options.fsx ")
    with pytest.raises(pysofamyroom.SofaMyRoomError,
                       match="missing field 'options.fs'"):
        simulate_text(missing_field)

    two_rows = re.sub(
        r"room\.surface\.absorption\s*=\s*\[[^\]]*\];",
        "room.surface.absorption = [0.1 0.1 0.1 0.1 0.1 0.1;"
        " 0.1 0.1 0.1 0.1 0.1 0.1];", sample_setup_text)
    with pytest.raises(pysofamyroom.SofaMyRoomError, match="6 surfaces"):
        simulate_text(two_rows)

    missing_sofa = re.sub(r"SOFA \S+\.sofa", "SOFA missing.sofa",
                          sample_setup_text)
    with pytest.raises(pysofamyroom.SofaMyRoomError, match="missing.sofa"):
        simulate_text(missing_sofa)

    # the library is still usable afterwards
    assert simulate_text(sample_setup_text)[0][0].shape[1] == 2


def test_log_callback(sample_setup_text):
    lines = []
    pysofamyroom.set_log(lambda text: lines.append(text.decode()))
    try:
        simulate_text(sample_setup_text)
    finally:
        pysofamyroom.set_log(None)
    assert any("Simulating specular" in line for line in lines)
    pysofamyroom.clear_sensors()
