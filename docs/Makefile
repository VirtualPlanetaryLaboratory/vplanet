# Makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    =
SPHINXBUILD   = sphinx-build
SPHINXPROJ    = vplanet
SOURCEDIR     = .
BUILDDIR      = .build

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@if [ $(MAKECMDGOALS) != clean ]; then \
		python gensrc.py; \
		python genexamples.py; \
		python gethelp.py; \
	else \
		rm -rf src.rst; \
		rm -rf src; \
		rm -rf examples.rst; \
		rm -rf examples; \
		rm -rf bugs; \
		rm -rf bugs.rst; \
		rm -rf help.rst; \
	fi
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O) 2>&1 | tee docs.log
	@if [ $(MAKECMDGOALS) != clean ]; then \
		mkdir -p $(BUILDDIR)/html/_images; \
		cp VPLanetLogo.png $(BUILDDIR)/html/_images/vplanet.png; \
	fi
