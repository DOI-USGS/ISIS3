#############################################################################
# Makefile for building: config Makefile
# Command: $(QMAKE) -o Makefile config.pro
#############################################################################
SHELL=bash
.SILENT:

QMAKE ?= $(shell which qmake 2 >& /dev/null)

ifeq ($(QMAKE),)
  QMAKE := $(shell which qmake-mac 2 >& /dev/null)
endif

ifeq ($(QMAKE),)
  ISISLOCALVERSION := $(shell head -n 3 $(ISISROOT)/version | tail -n 1 | sed 's/\#.*//' | sed 's/ *$$//')
  QMAKE := $(wildcard /opt/usgs/$(ISISLOCALVERSION)/ports/bin/qmake)
endif

ifeq ($(QMAKE),)
  QMAKE := $(wildcard /opt/usgs/$(ISISLOCALVERSION)/ports/bin/qmake-mac)
endif

ifeq ($(QMAKE),)
  QMAKE := $(wildcard /usgs/pkgs/local/$(ISISLOCALVERSION)/bin/qmake)
endif

ifeq ($(QMAKE),)
  QMAKE := $(wildcard /opt/local/bin/qmake-mac)
endif

config: Makefile

Makefile: FORCE config.pro
	if [ -f "$(QMAKE)" ]; then       \
	  $(QMAKE) -o Makefile config.pro; \
	  $(MAKE) isis_conf;               \
	fi;


clean:
	if [ -f Makefile ]; then \
	  $(MAKE) distclean;    \
	fi;

FORCE:

