from setuptools import setup
from Cython.Build import cythonize
from distutils.extension import Extension


# Needed extensions with roomsim.c to make it work.
extensions = [Extension("pyroomsim", ["pyroomsim.pyx", "roomsim.c"])]
setup(
    ext_modules=cythonize(extensions, language_level=3, annotate=True),
)

# python setup.py build_ext --inplace