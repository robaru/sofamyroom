from pathlib import Path

import numpy as np
import pytest

import pysofamyroom as smr

REPO = Path(__file__).resolve().parents[1]


def test_defaults_match_sample_setup(sample_setup_text, sofa_path):
    """The dataclass defaults are the values of data/sampleroomsetup.txt."""
    parsed = smr.Setup.from_text(sample_setup_text)
    default = smr.Setup(
        sources=[smr.Source((8, 2.5, 1.6), (180, 0, 0), "subcardioid")],
        receivers=[smr.Receiver((3, 5, 1.2), description=f"SOFA {sofa_path}")],
    )
    default.options.extra = parsed.options.extra  # mex_saveaswav
    assert parsed.to_text() == default.to_text()


def test_text_round_trip(sample_setup_text):
    setup = smr.Setup.from_text(sample_setup_text)
    again = smr.Setup.from_text(setup.to_text())
    assert again.to_text() == setup.to_text()
    # and the text is accepted by the C parser with identical results
    a = smr.simulate(sample_setup_text)[0][0]
    b = smr.simulate(setup)[0][0]
    assert np.array_equal(a, b)


@pytest.mark.usefixtures("sofa_path")
def test_read_setup_file_and_path_input(monkeypatch):
    monkeypatch.chdir(REPO)  # the sample refers to data/...sofa relatively
    setup = smr.read_setup(REPO / "data" / "sampleroomsetup.txt")
    assert setup.room.dimension == (10.0, 7.0, 4.0)
    assert setup.options.fs == 44100
    assert setup.sources[0].description == "subcardioid"
    a = smr.simulate(REPO / "data" / "sampleroomsetup.txt")[0][0]
    assert a.shape == (55125, 2)


def test_surface_validation():
    with pytest.raises(ValueError, match=r"\(6, 6\)"):
        smr.Surface(absorption=np.zeros((5, 6)))
    surface = smr.Surface(frequency=[125, 250], absorption=[0.1, 0.2])
    assert surface.absorption.shape == (6, 2)
    with pytest.raises(ValueError, match="three values"):
        smr.Room(dimension=(1, 2))


def test_receiver_sofa_helper(sofa_path):
    receiver = smr.Receiver.sofa(sofa_path, (1, 2, 1.5), interp=True,
                                 delay=True)
    assert receiver.description == f'SOFA "{sofa_path}" interp=1 delay=1'
    plain = smr.Receiver.sofa(sofa_path, (1, 2, 1.5))
    assert plain.description == f'SOFA "{sofa_path}"'


def test_sofa_path_with_spaces(sofa_path, tmp_path):
    folder = tmp_path / "dir with spaces"
    folder.mkdir()
    copy = folder / sofa_path.name
    copy.write_bytes(sofa_path.read_bytes())
    setup = smr.Setup(
        options=smr.Options(verbose=False),
        sources=[smr.Source((8, 2.5, 1.6))],
        receivers=[smr.Receiver.sofa(copy, (3, 5, 1.2))],
    )
    assert smr.simulate(setup)[0][0].shape == (55125, 2)
    # unquoted, the path is cut at the first space: a clear error, no crash
    setup.receivers[0].description = f"SOFA {copy}"
    with pytest.raises(smr.SofaMyRoomError, match="not found"):
        smr.simulate(setup)
    setup.receivers[0].description = f'SOFA "{folder}"'
    with pytest.raises(smr.SofaMyRoomError, match="is a directory"):
        smr.simulate(setup)


def test_multiple_sources_and_receivers(sofa_path):
    setup = smr.Setup(
        options=smr.Options(verbose=False, responseduration=0.3),
        sources=[smr.Source((8, 2.5, 1.6)), smr.Source((2, 2, 1.6))],
        receivers=[smr.Receiver.sofa(sofa_path, (3, 5, 1.2)),
                   smr.Receiver((5, 5, 1.2), description="omnidirectional")],
    )
    brirs = smr.simulate(setup)
    assert len(brirs) == 2 and len(brirs[0]) == 2
    assert brirs[0][0].shape == (int(0.3 * 44100), 2)
    assert brirs[0][1].shape == (int(0.3 * 44100), 1)
    # the omnidirectional receiver sees the two sources differently
    assert not np.array_equal(brirs[0][1], brirs[1][1])


def test_seed_controls_the_diffuse_tail(sofa_path):
    def run(seed):
        setup = smr.Setup(
            options=smr.Options(verbose=False, responseduration=0.3,
                                simulatediffuse=True, numberofrays=500,
                                seed=seed),
            sources=[smr.Source((8, 2.5, 1.6))],
            receivers=[smr.Receiver.sofa(sofa_path, (3, 5, 1.2))],
        )
        return smr.simulate(setup)[0][0]

    assert np.array_equal(run(7), run(7))
    assert not np.array_equal(run(7), run(8))


def test_invalid_setup_raises_before_running():
    with pytest.raises(ValueError, match="at least one source"):
        smr.Setup().to_text()
