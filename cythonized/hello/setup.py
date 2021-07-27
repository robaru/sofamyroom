from distutils.core import setup, Extension
from Cython.Distutils import build_ext

setup(
    cmdclass={'build_ext': build_ext},
    ext_modules=[Extension("phello", sources=["phello.pyx", "hello.c"], )]
)


# from setuptools import setup
# from Cython.Build import cythonize
#
# setup(
#     ext_modules=cythonize("phello.pyx", language_level=3, annotate=True),
# )
