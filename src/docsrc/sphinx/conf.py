# Configuration file for the Sphinx documentation builder.
#
# Build with `pip install .[docs]` and
# `sphinx-build -b html src/docsrc/sphinx <output>`. The Python pages use
# autodoc, so the pysofamyroom package must be importable; the C pages use
# hawkmoth, which reads the comments of the headers through libclang.

import os

# -- Project information -----------------------------------------------------
project = 'SofaMyRoom'
copyright = '2019-2026, Roberto Barumerli, Daniele Bianchi'
author = 'Roberto Barumerli, Daniele Bianchi'

try:
    import pysofamyroom
    release = version = pysofamyroom.__version__
except ImportError:  # docs of the C code only
    release = version = ''

# -- General configuration ---------------------------------------------------
extensions = [
    'myst_parser',
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'sphinx.ext.intersphinx',
    'hawkmoth',
    'hawkmoth.ext.javadoc',
]

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', '.nojekyll']

# C reference from the header comments (hawkmoth)
_src = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
hawkmoth_root = _src
hawkmoth_clang = ['-I' + os.path.join(_src, 'libroomsim', 'include'),
                  '-I' + os.path.join(_src, 'libmysofa', 'hrtf')]
hawkmoth_transform_default = 'javadoc'


def _drop_banner_lines(app, lines, transform, options):
    # the file comments end with a row of asterisks (Roomsim banner style)
    lines[:] = [line for line in lines if line.strip(' \t*')]


def setup(app):
    app.connect('hawkmoth-process-docstring', _drop_banner_lines)

autodoc_member_order = 'bysource'
autodoc_typehints = 'description'
napoleon_numpy_docstring = True
napoleon_google_docstring = False
napoleon_use_rtype = False

intersphinx_mapping = {
    'python': ('https://docs.python.org/3', None),
    'numpy': ('https://numpy.org/doc/stable/', None),
    'scipy': ('https://docs.scipy.org/doc/scipy/', None),
}

myst_heading_anchors = 3

master_doc = 'index'

# -- Options for HTML output -------------------------------------------------
html_theme = 'sphinx_rtd_theme'
html_static_path = []
