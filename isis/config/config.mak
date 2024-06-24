#############################################################################
# Makefile for building: config Makefile
# Command: $(QMAKE) -o Makefile config.pro
#############################################################################
SHELL=bash
.SILENT:

ISISLOCALVERSION := $(shell head -n 3 $(ISISROOT)/isis_version.txt | tail -n 1 | sed 's/\#.*//' | sed 's/ *$$//')
QMAKE ?= $(shell which qmake 2 >& /dev/null)

ifeq ($(QMAKE),)
  QMAKE := $(shell which qmake-mac 2 >& /dev/null)
endif

ifeq ($(QMAKE),)
  QMAKE := $(wildcard /opt/usgs/$(ISISLOCALVERSION)/ports/bin/qmake)
endif

ifeq ($(QMAKE),)
  QMAKE := $(wildcard /opt/usgs/$(ISISLOCALVERSION)/ports/libexec/qt5/bin/qmake)
#  QMAKEPARMS := -nocache
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
	  $(QMAKE) $(QMAKEPARMS) -o Makefile config.pro 2>&1 | grep -v MESSAGE; \
	  $(MAKE) isis_conf;               \
	fi;


clean:
	if [ -f Makefile ]; then \
	  $(MAKE) distclean;    \
	fi;

FORCE:

