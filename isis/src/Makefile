include $(ISISROOT)/make/isismake.macros

PACKAGES = $(filter-out Makefile docsys CVS IsisObjectConvert.xsl, $(wildcard *))

applications: 
	for i in $(PACKAGES); do \
          printf %-46s "  Working on Package [$$i]"; \
	  cd $$i; $(MAKE) applications; cd ..; \
	done

api: 
	for i in $(PACKAGES); do \
          echo "  Working on Package [$$i] ("`date +%T`")"; \
	  cd $$i; $(MAKE) api; cd ..; \
	done

objects: 
	for i in $(PACKAGES); do \
          echo "  Working on Package [$$i]"; \
	  cd $$i; $(MAKE) objects; cd ..; \
	done
# Saved for inside if
#	  cd $$i; $(MAKE) clean; $(MAKE) objects; cd ..; \

includes:
	for i in $(PACKAGES); do \
          echo "  Working on Package [$$i] ("`date +%T`")"; \
	  cd $$i; $(MAKE) includes; cd ..; \
	done

documentation:
	for i in $(PACKAGES); do \
          printf %-46s "  Working on Package [$$i]"; \
	  cd $$i; $(MAKE) documentation; cd ..; \
	done

clean:
	for i in $(PACKAGES); do \
	  echo "  Working on Package [$$i]"; \
	  cd $$i; $(MAKE) clean; cd ..; \
	done
	cd docsys; $(MAKE) clean; cd ..;

unitTest:
	for i in $(PACKAGES); do \
	  if [ "$$i" != "qisis" ]; \
	  then \
	    echo "  Working on Package [$$i]"; \
	    cd $$i; $(MAKE) unitTest; cd ..; \
	  fi \
	done

appTest:
	for i in $(PACKAGES); do \
          printf %-46s "  Working on Package [$$i]"; \
	  cd $$i; $(MAKE) appTest; cd ..; \
	done

catTest:
	for i in $(PACKAGES); do \
          printf %-46s "  Working on Package [$$i]"; \
	  cd $$i; $(MAKE) catTest; cd ..; \
	done
