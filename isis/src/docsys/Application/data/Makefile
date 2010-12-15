include $(ISISROOT)/make/isismake.macros

# ISIS APPLICATION DOCS MAKEFILE
#   Process the XML files deposited by application
#   makefiles
#   dls - deborah lee soltesz - 4/25/2002


DEST_PRES_DIR = $(ISISROOT)/doc/Application/presentation

PFSTYLEDIR = ../presentation/PrinterFriendly/styles
PFDIR = $(DEST_PRES_DIR)/PrinterFriendly
TABSTYLEDIR = ../presentation/Tabbed/styles
TABDIR = $(DEST_PRES_DIR)/Tabbed

APPLICATIONS = $(filter-out CVS $(wildcard *.*) $(wildcard Makefile*) , $(wildcard *))

help:
	echo "Do not run make in this directory!" 
	echo "Run make from $ISISROOT/src/docsys/Application to remake Application documentation."


APPSDIR = $(ISISROOT)/doc/Application

#------
# DOCS: generate documentation

docs: printer tabbed toc


#------
# PRINTER FRIENDLY
# run style sheet for Printer Friendly presentation
printer:

	echo "        Printer Friendly Version"

	for dirname in $(APPLICATIONS); do \
	  cd $$dirname ; make -f ../Makefile-application printer ; cd .. ; \
	done
	mkdir -p $(PFDIR)/styles ; \
	rsync -lHptr --delete --rsh=ssh $(PFSTYLEDIR)/*.css $(PFDIR)/styles/ ; \

#------
# TABBED
# run style sheet for Tabbed presentation
tabbed:

	echo "        Tabbed Interface Version"

	for dirname in $(APPLICATIONS); do \
 	    cd $$dirname ; make -f ../Makefile-application tabbed ; cd .. ; \
	done
	mkdir -p $(TABDIR)/styles ; \
	rsync -lHptr --delete --rsh=ssh $(TABSTYLEDIR)/*.css $(TABDIR)/styles/ ; \


#------
# TOC
# run style sheet for Table of Contents XML
toc: ../build/toc.xml
../build/toc.xml: $(APPSDIR) ../build/IsisApplicationTOCbuild.xsl

	echo "        Table of Contents XML"
	cat ../build/toc_header.xml > ../build/toc.xml
	for dirname in $(APPLICATIONS); do \
	  echo "          Adding [$$dirname]" ; \
	  $(XALAN) $$dirname/$$dirname.xml ../build/IsisApplicationTOCbuild.xsl >> ../build/toc.xml ; \
	done
	cat ../build/toc_footer.xml >> ../build/toc.xml
	mkdir -p $(ISISROOT)/bin/xml/
	cp ../build/toc.xml $(ISISROOT)/bin/xml/applicationTOC.xml

#------
# CLEAN: clean up files

clean:
	rm -rf $(APPLICATIONS) 




