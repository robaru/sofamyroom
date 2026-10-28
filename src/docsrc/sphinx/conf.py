# Configuration file for the Sphinx documentation builder.
#
# Build with `pip install .[docs]` and
# `sphinx-build -b html src/docsrc/sphinx <output>`; the Python pages use
# autodoc, so the pysofamyroom package must be importable.

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
]

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', '.nojekyll']

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
