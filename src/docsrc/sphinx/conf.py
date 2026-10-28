# Configuration file for the Sphinx documentation builder.
#
# Two ways to build:
#   * `cmake -DBUILD_DOCS=True`: Doxygen runs first and the C reference page
#     (doxydoc.rst) is rendered through breathe; CMake passes the location
#     of the Doxygen XML in SOFAMYROOM_DOXYGEN_XML.
#   * `pip install .[docs]` + `sphinx-build` (Read the Docs): the Python
#     pages are built with autodoc, the C reference page is skipped.

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
]

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', '.nojekyll']

# C reference through Doxygen + breathe, only when the XML output exists;
# otherwise the doxydoc page says how to build it
doxygen_xml = os.environ.get('SOFAMYROOM_DOXYGEN_XML', '')
have_doxygen = bool(doxygen_xml) and os.path.isdir(doxygen_xml)
if have_doxygen:
    extensions.append('breathe')
    breathe_projects = {'SofaMyRoom': doxygen_xml}
    breathe_default_project = 'SofaMyRoom'


def setup(app):
    if have_doxygen:
        return
    from docutils import nodes
    from docutils.parsers.rst import Directive

    class NoDoxygen(Directive):
        def run(self):
            text = ('The C code reference is generated with Doxygen and '
                    'Breathe when the documentation is built through CMake '
                    '(cmake -DBUILD_DOCS=True); it is not part of this '
                    'build.')
            return [nodes.note('', nodes.paragraph(text=text))]

    app.add_directive('doxygenindex', NoDoxygen)

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
