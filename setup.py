from setuptools import *

extensions = [Extension("bstree", sources=["bstree.c"])]
setup(name="bstree", version="1.0", ext_modules=extensions)