from setuptools import setup
from Cython.Build import cythonize

setup(
    ext_modules=cythonize("primes.pyx", language_level=3, annotate=True),
)

# python setup.py build_ext --inplace
