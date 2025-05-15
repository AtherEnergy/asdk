# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'ASDK'
copyright = '2025, Ather Energy Limited'
author = 'Ather Energy Limited'
release = '2025'

smv_branch_whitelist = None
smv_tag_whitelist = r'^.*$'
smv_remote_whitelist = r'^(origin)$'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['sphinx_multiversion', 'breathe', 'myst_parser', "sphinx.ext.autodoc"]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

breathe_default_project = 'ASDK'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_extra_path = ['resources']

breathe_projects = {
    "ASDK"          : "./doxygen_output/xml",
}

breathe_domain_by_extension = {
    "h" : "c",
    "c" : "c",
}
