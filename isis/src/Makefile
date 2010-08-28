include $(ISISROOT)/make/isismake.macros

# Although we're capable of parallel builds at this level, let's not for the
# sake of the logs - keep apps & objects under the correct package.
.NOTPARALLEL:

PACKAGES = $(filter-out Makefile docsys CVS IsisObjectConvert.xsl, $(wildcard *))

PACKAGESCLEAN := $(PACKAGES:%=%-clean)
PACKAGESINCS := $(PACKAGES:%=%-includes)
PACKAGESOBJS := $(PACKAGES:%=%-objects)
PACKAGESAPPS := $(PACKAGES:%=%-apps)
PACKAGESDOCS := $(PACKAGES:%=%-docs)
PACKAGESUNITTEST := $(filter-out qisis-unitTest, $(PACKAGES:%=%-unitTest))
PACKAGESAPPTEST := $(filter-out qisis-appTest,   $(PACKAGES:%=%-appTest))
PACKAGESCATTEST := $(filter-out qisis-catTest,   $(PACKAGES:%=%-catTest))

# Allow parallel builds of applications
applications: $(PACKAGESAPPS)
	
%-apps:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-apps//'`; \
	echo "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE applications

# We need this to be serialized
api:
	for i in $(PACKAGES); do \
	  echo "  Working on Package [$$i] ("`date +%T`")"; \
	  cd $$i; $(MAKE) api; cd ..; \
	done

objects: $(PACKAGESOBJS)

%-objects:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-objects//'`; \
	echo "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE objects

includes: $(PACKAGESINCS)

%-includes:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-includes//'`; \
	echo "  Working on Package [$$PACKAGE] ("`date +%T`")"; \
	$(MAKE) --directory=$$PACKAGE includes;

documentation: $(PACKAGESDOCS)

%-docs:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-docs//'`; \
	echo "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE documentation;

clean: $(PACKAGESCLEAN)
	$(MAKE) --directory=docsys clean;

%-clean:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-clean//'`; \
	echo "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE clean;


unitTest: $(PACKAGESUNITTEST)

%-unitTest:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-unitTest//'`; \
	echo "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE unitTest;


appTest: $(PACKAGESAPPTEST)

%-appTest:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-appTest//'`; \
	echo "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE appTest;


catTest: $(PACKAGESCATTEST)

%-catTest:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-catTest//'`; \
	echo "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE catTest;