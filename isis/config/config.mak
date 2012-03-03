#############################################################################
# Makefile for building: config Makefile
# Command: $(QMAKE) -o Makefile config.pro
#############################################################################
QMAKE ?= $(shell which qmake)

ifeq ($(QMAKE),)
  QMAKE := $(shell which qmake-mac)
endif

ifeq ($(QMAKE),)
  ISISLOCALVERSION := $(shell head -n 3 $(ISISROOT)/version | tail -n 1 | sed 's/\#.*//' | sed 's/ *$$//')
  QMAKE := $(wildcard /opt/usgs/$(ISISLOCALVERSION)/ports/bin/qmake)
endif

ifeq ($(QMAKE),)
  QMAKE := $(wildcard /opt/usgs/$(ISISLOCALVERSION)/ports/bin/qmake-mac)
endif

ifeq ($(QMAKE),)
  QMAKE := /opt/local/bin/qmake-mac
endif

config: Makefile

Makefile: FORCE config.pro
	@$(QMAKE) -o Makefile config.pro
	@$(MAKE) isis_conf


clean:
	@$(MAKE) distclean

FORCE:

