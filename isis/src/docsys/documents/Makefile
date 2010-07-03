include $(ISISROOT)/make/isismake.macros

# ISIS DOCS MAKEFILE
#   Create XML table of contents for the documents
#   and generate upper level navigation pages
#   dls - deborah lee soltesz - 12/04/2002


DOCUMENTS    = $(filter-out CVS $(wildcard Makefile*) $(wildcard *.*) , $(wildcard *))
DOCDIR       = $(ISISROOT)/doc/documents




docs: documents tocXML aboutisis general guides installation technicalinfo userdocs



# DOCUMENT LIST
# build individual documents as needed

documents:
	echo "        Building Individual Documents"
	for dirname in $(DOCUMENTS); do \
	  cd $$dirname ; \
	  make -f ../Makefile-documents docbuild ; \
	  cd .. ; \
	done
	echo "        Finished Building Individual Documents"



# TABLE OF CONTENTS XML
# build tables of contents XML file for doc directories

tocXML: ../build/doctoc.xml ../build/IsisDocumentTOCbuild.xsl
../build/doctoc.xml: $(DOCDIR)

	echo "        Building Table of Contents XML"
	cat ../build/doctoc_header.xml >  ../build/doctoc.xml
	for dirname in $(DOCUMENTS); do \
	  echo "          Adding [$$dirname]" ; \
	  $(XALAN) -p dirParam "'$$dirname'" $$dirname/$$dirname.xml ../build/IsisDocumentTOCbuild.xsl >> ../build/doctoc.xml ; \
	done
	cat ../build/doctoc_footer.xml >> ../build/doctoc.xml
	echo "        Finished Building Table of Contents XML"



# TABLE OF CONTENTS PAGES
# build various upper level pages that link documents

# ABOUT ISIS TOC
aboutisis: $(ISISROOT)/doc/AboutIsis/index.html
$(ISISROOT)/doc/AboutIsis/index.html: ../build/doctoc.xml ../build/AboutIsis.xsl

	echo "        Constructing [About Isis]"
	$(XALAN) -p menuPath "'../'" -o $(ISISROOT)/doc/AboutIsis/index.html     ../build/doctoc.xml ../build/AboutIsis.xsl


# GENERAL TOC
general: $(ISISROOT)/doc/General/index.html
$(ISISROOT)/doc/General/index.html: ../build/doctoc.xml ../build/General.xsl

	echo "        Constructing [General]"
	$(XALAN) -p menuPath "'../'" -o $(ISISROOT)/doc/General/index.html       ../build/doctoc.xml ../build/General.xsl

# GUIDES TOC
guides: $(ISISROOT)/doc/Guides/index.html
$(ISISROOT)/doc/Guides/index.html: ../build/doctoc.xml ../build/Guides.xsl

	echo "        Constructing [Guides]"
	$(XALAN)  -p menuPath "'../'" -o $(ISISROOT)/doc/Guides/index.html       ../build/doctoc.xml ../build/Guides.xsl

# INSTALLATION TOC
installation: $(ISISROOT)/doc/Installation/index.html
$(ISISROOT)/doc/Installation/index.html: ../build/doctoc.xml ../build/Installation.xsl

	echo "        Constructing [Installation]"
	$(XALAN) -p menuPath "'../'" -o $(ISISROOT)/doc/Installation/index.html  ../build/doctoc.xml ../build/Installation.xsl

# TECHNICAL INFO TOC
technicalinfo: $(ISISROOT)/doc/TechnicalInfo/index.html
$(ISISROOT)/doc/TechnicalInfo/index.html: ../build/doctoc.xml ../build/TechnicalInfo.xsl

	echo "        Constructing [TechnicalInfo]"
	$(XALAN) -p menuPath "'../'" -o $(ISISROOT)/doc/TechnicalInfo/index.html ../build/doctoc.xml ../build/TechnicalInfo.xsl

# USER DOCS TOC
userdocs: $(ISISROOT)/doc/UserDocs/index.html
$(ISISROOT)/doc/UserDocs/index.html: ../build/doctoc.xml ../build/UserDocs.xsl

	echo "        Constructing [UserDocs]"
	$(XALAN) -p menuPath "'../'" -o $(ISISROOT)/doc/UserDocs/index.html      ../build/doctoc.xml ../build/UserDocs.xsl






# CLEAN
clean:
	for dirname in $(DOCUMENTS); do \
	    cd $$dirname ; \
	    rm -f *.html ; \
	    rm -f Makefile-temp ; \
	    cd .. ; \
	done




