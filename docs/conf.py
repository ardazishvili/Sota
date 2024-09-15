# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'Sota'
copyright = '2024, Roman Ardazishvili and Contributors'
author = 'Roman Ardazishvili and Contributors'
release = '0.11-beta'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'myst_parser',
    'breathe',
    'exhale',
    'sphinxcontrib.plantuml',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------
html_theme = 'sphinx_rtd_theme'

html_static_path = ['_static']

source_suffix = {
        '.rst': "restructuredtext",
        '.md': "markdown",
}


# Setup the breathe extension
breathe_projects = {
    "Sota": "./_doxygen/xml"
}
breathe_default_project = "Sota"

# Setup the exhale extension
exhale_args = {
    "containmentFolder":     "./api",
    "rootFileName":          "library_root.rst",
    "doxygenStripFromPath":  "..",
    "rootFileTitle":         "Library API",
    "createTreeView":        True,
    "exhaleExecutesDoxygen": True,
    "exhaleUseDoxyfile": True
}
primary_domain = 'cpp'

on_rtd = os.environ.get('READTHEDOCS') == 'True'
if not on_rtd:
    directory = os.environ.get('PLANTUML_JAR_DIR')
    if not directory:
        print("Please set PLANTUML_JAR_DIR system variable and try again")
        sys.exit()
    plantuml = ["java", "-jar", os.path.join(directory, "plantuml.jar")]
