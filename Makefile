include Makefile.config

SUBDIRS = \
  src \
  doc

# The code in the test directory does not build.

default: all
.PHONY: default

all:
	for subdir in $(SUBDIRS); do \
		( cd $$subdir && make -$(MAKEFLAGS) all ) || exit 1; \
	done
.PHONY: all

clean:
	for subdir in $(SUBDIRS); do \
		( cd $$subdir && make -$(MAKEFLAGS) clean ) || exit 1; \
	done
.PHONY: clean

