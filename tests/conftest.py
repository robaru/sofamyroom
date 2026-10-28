import urllib.request
from pathlib import Path

import pytest

REPO = Path(__file__).resolve().parents[1]
SOFA_URL = ("https://sofacoustics.org/data/database/mit/"
            "mit_kemar_normal_pinna.sofa")


@pytest.fixture(scope="session")
def sofa_path() -> Path:
    """MIT KEMAR SOFA file, downloaded once into data/ if missing."""
    path = REPO / "data" / "MIT_KEMAR_normal_pinna.sofa"
    if not path.is_file():
        path.parent.mkdir(exist_ok=True)
        urllib.request.urlretrieve(SOFA_URL, path)
    return path


@pytest.fixture(scope="session")
def sample_setup_text(sofa_path) -> str:
    """Text of data/sampleroomsetup.txt with an absolute SOFA path."""
    text = (REPO / "data" / "sampleroomsetup.txt").read_text()
    return text.replace("data/MIT_KEMAR_normal_pinna.sofa", str(sofa_path))
