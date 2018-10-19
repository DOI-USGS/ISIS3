include $(ISISROOT)/make/isismake.os

# Although we're capable of parallel builds at this level, let's not for the
# sake of the logs - keep apps & objects under the correct package.
ifeq ($(findstring MULTIPACKAGE, $(MODE)),MULTIPACKAGE)
else
.NOTPARALLEL:
endif

PACKAGES = $(filter-out Makefile CMakeLists.txt docsys CVS IsisObjectConvert.xsl, $(wildcard *))

PACKAGESCLEAN := $(PACKAGES:%=%-clean)
PACKAGESQUICKCLEAN := $(PACKAGES:%=%-quickclean)
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
	echo $(CURTIMESTAMP) "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE applications

# We need this to be serialized
api:
	for i in $(PACKAGES); do \
	  echo $(CURTIMESTAMP) "  Working on Package [$$i]"; \
	  cd $$i; $(MAKE) api; cd ..; \
	done

objects: $(PACKAGESOBJS)

%-objects:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-objects//'`; \
	echo $(CURTIMESTAMP) "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE objects

includes: $(PACKAGESINCS)

%-includes:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-includes//'`; \
	echo $(CURTIMESTAMP) "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE includes;

documentation: $(PACKAGESDOCS)

%-docs:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-docs//'`; \
	echo $(CURTIMESTAMP) "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE documentation;

quickclean: $(PACKAGESQUICKCLEAN)

%-quickclean:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-quickclean//'`; \
	echo $(CURTIMESTAMP) "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE quickclean;

clean: $(PACKAGESCLEAN)
	$(MAKE) --directory=docsys clean;

%-clean:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-clean//'`; \
	echo $(CURTIMESTAMP) "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE clean;


unitTest: $(PACKAGESUNITTEST)

%-unitTest:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-unitTest//'`; \
	echo $(CURTIMESTAMP) "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE unitTest;


appTest: $(PACKAGESAPPTEST)

%-appTest:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-appTest//'`; \
	echo $(CURTIMESTAMP) "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE appTest;


catTest: $(PACKAGESCATTEST)

%-catTest:
	PACKAGE=`$(ECHO) $@ | $(SED) 's/-catTest//'`; \
	echo $(CURTIMESTAMP) "  Working on Package [$$PACKAGE]"; \
	$(MAKE) --directory=$$PACKAGE catTest;
