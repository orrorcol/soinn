# This file is taken from https://github.com/nmslib/hnswlib/blob/master/setup.py

import os
import sys

import numpy as np
import pybind11
import setuptools
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

__version__ = '0.2.1'


include_dirs = [
    pybind11.get_include(),
    np.get_include(),
]

# compatibility when run in python_bindings
bindings_dir = 'python'
source_files = ['./python/soinn.cpp']
include_dirs.extend(['./soinn'])


libraries = []
extra_objects = []


ext_modules = [
    Extension(
        'soinn',
        source_files,
        include_dirs=include_dirs,
        libraries=libraries,
        language='c++',
        extra_objects=extra_objects,
    ),
]


def cpp_flag(compiler):
    """Return the -std=c++[11/14] compiler flag.
    The c++14 is prefered over c++11 (when it is available).
    """
    return '-std=c++14'


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc', '/openmp', '/O2'],
        'unix': ['-O3', '-march=native'],  # , '-w'
    }
    link_opts = {
        'unix': [],
        'msvc': [],
    }

    if sys.platform == 'darwin':
        c_opts['unix'] += ['-stdlib=libc++', '-mmacosx-version-min=10.7']
        link_opts['unix'] += ['-stdlib=libc++', '-mmacosx-version-min=10.7']
    else:
        c_opts['unix'].append("-fopenmp")
        link_opts['unix'].extend(['-fopenmp', '-pthread'])

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append(cpp_flag(self.compiler))
            opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())

        for ext in self.extensions:
            ext.extra_compile_args.extend(opts)
            ext.extra_link_args.extend(self.link_opts.get(ct, []))

        build_ext.build_extensions(self)


setup(
    name='soinn',
    version=__version__,
    description='soinn',
    author='lfs',
    url='https://github.com/uestc-lfs/soinn',
    long_description="""soinn impl""",
    ext_modules=ext_modules,
    install_requires=['numpy'],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,
)
