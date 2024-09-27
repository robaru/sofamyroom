from setuptools import setup, find_packages
from Cython.Build import cythonize
from distutils.extension import Extension

pysofamyroom_version = "0.0.1"

rs_files = ["roomsim.c", "sensor.c", "dsp.c", "3D.c", "interp.c", "rng.c"]
rs_files = ["src/libroomsim/source/" + r for r in rs_files]
src = ["pysofamyroom/pysofamyroom.pyx", "pysofamyroom/printer.c", "src/libsfmt/SFMT.c"] + rs_files
inc_dirs = ['pysofamyroom', 'src/libroomsim/include', "src/libmysofa/Linux/64bit/include",
            "src/libsfmt", 'src/libroomsim/source']
# Needed extensions with roomsim.c to make it work.
# extensions = [Extension("pyroomsim", ["pyroomsim.pyx", "roomsim.c"])]
extensions = [Extension(
    name="pysofamyroom.internal", sources=src, include_dirs=inc_dirs,
    libraries=["fftw3", "z"],
    extra_objects=["./src/libmysofa/Linux/64bit/lib/Release/libmysofa.a"]
    # library_dirs=[],
    # libraries=["libroomsim", "libsfmt", "sofamyroom"],
    # library_dirs=["build/", "build/libroomsim/", "build/libsfmt/"],
    # library_dirs=["build/", "build/libroomsim/CMakeFiles/libroomsim.dir/source/",
    #               "build/libsfmt/CMakeFiles/libsfmt.dir/", "build/CMakeFiles/sofamyroom.dir/"]
)]
# setup(
#     ext_modules=cythonize(extensions, language_level=3, annotate=True),
#     zip_safe=False,
# )

# python setup.py build_ext --inplace



setup(
    name="pysofamyroom",
    version=pysofamyroom_version,
    author="Manuel Pariente & Prerak Srivastava",
    author_email="manuel.pariente@loria.fr",
    url="",
    description="",
    long_description="",
    long_description_content_type="text/markdown",
    license="",
    python_requires=">=3.6",
    install_requires=[
        "pyyaml",
        "numpy",
        "soundfile",
    ],
    packages=find_packages(),
    ext_modules=cythonize(extensions, language_level=3, annotate=True),
    zip_safe=False,
)
