from setuptools import *

extensions = [Extension("bstree", sources=["bstree.c"])]
setup(
    name="bstree",
    version="0.5",
    description="Binary search tree",
    long_description=open("README.md", encoding="utf-8").read(),
    long_description_content_type="text/markdown",
    author="nikaided",
    author_email="nikaided@gmail.com",
    ext_modules=extensions,
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
    ],
)
