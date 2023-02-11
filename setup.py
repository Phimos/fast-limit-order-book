from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

ext_modules = [
    Pybind11Extension(
        "flob",
        ["src/flob.cpp"],
        include_dirs=["include"],
        cxx_std=17,
        extra_compile_args=["-O3"],  # -Ofast may cause precision error
    )
]

setup(
    name="flob",
    version="1.0.0",
    description="fast-limit-order-book",
    author="Yunchong Gan",
    author_email="ganyunchong@gmail.com",
    url="https://github.com/Phimos/fast-limit-order-book",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
