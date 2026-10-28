# Contributing to sofamyroom

Thanks for your interest in sofamyroom.

## Reporting problems

Open an issue at https://github.com/robaru/sofamyroom/issues and include:

- your operating system and whether you use the command-line tool or the MATLAB MEX file,
- the room setup file (or MATLAB struct) that triggers the problem,
- the SOFA file you used, if the problem involves HRTFs,
- the full output printed by sofamyroom.

## Asking for help

Questions about usage are welcome as GitHub issues too. Please read the
documentation at https://robaru.github.io/sofamyroom first.

## Proposing changes

1. Fork the repository and create a branch from `master`.
2. Build with CMake (see the README) and make sure `unittest` still passes.
3. Keep the C code warning-free under `-Wall -Wextra`.
4. For the Python package: `pip install -e ".[tests]"`, then `ruff check python tests` and `pytest` must pass. Python code follows the numpy docstring style (configured in `pyproject.toml`).
5. Open a pull request describing what changed and why.

Changes that alter the simulated output must be opt-in options that default to the previous behaviour, so that published results stay reproducible.

By contributing you agree that your contribution is released under the
EUPL-1.2 licence of this project (see `LICENSE.txt`).
