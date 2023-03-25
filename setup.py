from setuptools import *

extensions = [Extension("bstree", sources=["bstree.c"])]
setup(name="bstree", version="0.1", ext_modules=extensions)