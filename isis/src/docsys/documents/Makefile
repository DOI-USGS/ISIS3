include $(ISISROOT)/make/isismake.os

.NOTPARALLEL:

# ISIS DOCS MAKEFILE
#   Create XML table of contents for the documents
#   and generate upper level navigation pages
#   dls - deborah lee soltesz - 12/04/2002


DOCUMENTS    = $(filter-out CVS $(wildcard Makefile*) $(wildcard *.*) , $(wildcard *))

RELEASENOTESPREREQ=$(wildcard ReleaseNotes/ReleaseNotesList.xml)

ifneq ($(RELEASENOTESPREREQ), ReleaseNotes/ReleaseNotesList.xml)
  DOCUMENTS    = $(filter-out CVS $(wildcard Makefile*) ReleaseNotes ParameterChanges ApiChanges $(wildcard *.*) , $(wildcard *))
endif

DOCUMENTXMLS   = $(foreach DOC,$(DOCUMENTS),$(DOC)/$(DOC).xml)

DOCDIR       = $(ISISROOT)/doc/documents


docs: tocXML
	$(MAKE) documents aboutisis general guides installation technicalinfo userdocs


# DOCUMENT LIST
# build individual documents as needed

documents:
	echo "        Building Individual Documents"
	for dirname in $(DOCUMENTS); do \
	  cd $$dirname ; \
	  $(MAKE) -f ../Makefile-documents docbuild ; \
	  cd .. ; \
	done
	echo "        Finished Building Individual Documents"



# TABLE OF CONTENTS XML
# build tables of contents XML file for doc directories

tocXML: ../build/doctoc.xml ../build/IsisDocumentTOCbuild.xsl $(DOCUMENTXMLS)
../build/doctoc.xml: $(DOCDIR) $(DOCUMENTXMLS)

	echo "        Building Table of Contents XML";                           \
	cat ../build/doctoc_header.xml >  ../build/doctoc.xml;                   \
	for dirname in $(DOCUMENTS); do                                          \
	  echo "          Adding [$$dirname]" ;                                  \
	  $(XALAN) $(XALAN_PARAM_OPTION) dirParam "'$$dirname'"                  \
				$(XALAN_INFILE_OPTION) $$dirname/$$dirname.xml $(XALAN_XSL_OPTION) \
				../build/IsisDocumentTOCbuild.xsl >> ../build/doctoc.xml ;         \
	done;                                                                    \
	cat ../build/doctoc_footer.xml >> ../build/doctoc.xml;                   \
	echo "        Finished Building Table of Contents XML"



# TABLE OF CONTENTS PAGES
# build various upper level pages that link documents

# ABOUT ISIS TOC
aboutisis: $(ISISROOT)/doc/AboutIsis/index.html
$(ISISROOT)/doc/AboutIsis/index.html: ../build/doctoc.xml ../build/AboutIsis.xsl

	echo "        Constructing [About Isis]"
	$(XALAN) $(XALAN_PARAM_OPTION) menuPath "'../'" $(XALAN_OUTFILE_OPTION) $(ISISROOT)/doc/AboutIsis/index.html   $(XALAN_INFILE_OPTION) ../build/doctoc.xml $(XALAN_XSL_OPTION) ../build/AboutIsis.xsl


# GENERAL TOC
general: $(ISISROOT)/doc/General/index.html
$(ISISROOT)/doc/General/index.html: ../build/doctoc.xml ../build/General.xsl

	echo "        Constructing [General]"
	$(XALAN) $(XALAN_PARAM_OPTION) menuPath "'../'" $(XALAN_OUTFILE_OPTION) $(ISISROOT)/doc/General/index.html      $(XALAN_INFILE_OPTION) ../build/doctoc.xml $(XALAN_XSL_OPTION) ../build/General.xsl

# GUIDES TOC
guides: $(ISISROOT)/doc/Guides/index.html
$(ISISROOT)/doc/Guides/index.html: ../build/doctoc.xml ../build/Guides.xsl

	echo "        Constructing [Guides]"
	$(XALAN)  $(XALAN_PARAM_OPTION) menuPath "'../'" $(XALAN_OUTFILE_OPTION) $(ISISROOT)/doc/Guides/index.html      $(XALAN_INFILE_OPTION) ../build/doctoc.xml $(XALAN_XSL_OPTION) ../build/Guides.xsl

# INSTALLATION TOC
installation: $(ISISROOT)/doc/Installation/index.html
$(ISISROOT)/doc/Installation/index.html: ../build/doctoc.xml ../build/Installation.xsl

	echo "        Constructing [Installation]"
	$(XALAN) $(XALAN_PARAM_OPTION) menuPath "'../'" $(XALAN_OUTFILE_OPTION) $(ISISROOT)/doc/Installation/index.html $(XALAN_INFILE_OPTION) ../build/doctoc.xml $(XALAN_XSL_OPTION) ../build/Installation.xsl

# TECHNICAL INFO TOC
technicalinfo: $(ISISROOT)/doc/TechnicalInfo/index.html
$(ISISROOT)/doc/TechnicalInfo/index.html: ../build/doctoc.xml ../build/TechnicalInfo.xsl

	echo "        Constructing [TechnicalInfo]"
	$(XALAN) $(XALAN_PARAM_OPTION) menuPath "'../'" $(XALAN_OUTFILE_OPTION) $(ISISROOT)/doc/TechnicalInfo/index.html $(XALAN_INFILE_OPTION) ../build/doctoc.xml $(XALAN_XSL_OPTION) ../build/TechnicalInfo.xsl

# USER DOCS TOC
userdocs: $(ISISROOT)/doc/UserDocs/index.html
$(ISISROOT)/doc/UserDocs/index.html: ../build/doctoc.xml ../build/UserDocs.xsl

	echo "        Constructing [UserDocs]"
	$(XALAN) $(XALAN_PARAM_OPTION) menuPath "'../'" $(XALAN_OUTFILE_OPTION) $(ISISROOT)/doc/UserDocs/index.html    $(XALAN_INFILE_OPTION) ../build/doctoc.xml $(XALAN_XSL_OPTION) ../build/UserDocs.xsl


ReleaseNotes/ReleaseNotes.xml: ../build/ReleaseNotes.xsl ReleaseNotes/ReleaseNotesList.xml
	if [ -f ReleaseNotes/ReleaseNotesList.xml ]; then \
		$(XALAN) $(XALAN_PARAM_OPTION) dirParam "'ReleaseNotes'"         \
					$(XALAN_INFILE_OPTION) ReleaseNotes/ReleaseNotesList.xml   \
					$(XALAN_XSL_OPTION) ../build/ReleaseNotes.xsl >            \
					ReleaseNotes/ReleaseNotes.xml ;                            \
	fi;


ParameterChanges/ParameterChanges.xml: ../build/ParameterChanges.xsl ReleaseNotes/ReleaseNotesList.xml
	if [ -f ReleaseNotes/ReleaseNotesList.xml ]; then                                         \
		$(XALAN) $(XALAN_PARAM_OPTION) dirParam "'ParameterChanges'"                      \
					$(XALAN_INFILE_OPTION) ReleaseNotes/ReleaseNotesList.xml  \
					$(XALAN_XSL_OPTION) ../build/ParameterChanges.xsl >       \
					ParameterChanges/ParameterChanges.xml ;                   \
	fi;


ApiChanges/ApiChanges.xml: ../build/ApiChanges.xsl ReleaseNotes/ReleaseNotesList.xml
	if [ -f ReleaseNotes/ReleaseNotesList.xml ]; then                                         \
		$(XALAN) $(XALAN_PARAM_OPTION) dirParam "'ApiChanges'"                      \
					$(XALAN_INFILE_OPTION) ReleaseNotes/ReleaseNotesList.xml  \
					$(XALAN_XSL_OPTION) ../build/ApiChanges.xsl >       \
					ApiChanges/ApiChanges.xml ;                   \
	fi;


ReleaseNotes/ReleaseNotesList.xml: FORCE

$(DOCUMENTXMLS):


# CLEAN
clean:
	for dirname in $(DOCUMENTS); do \
	    cd $$dirname ; \
	    rm -f *.html ; \
	    rm -f Makefile-temp ; \
	    cd .. ; \
	done ; \
	if [ -f ReleaseNotes/ReleaseNotes.xml ]; then \
	  rm ReleaseNotes/ReleaseNotes.xml; \
	fi; \
	if [ -f ApiChanges/ApiChanges.xml ]; then \
	  rm ApiChanges/ApiChanges.xml; \
	fi; \
	if [ -f ParameterChanges/ParameterChanges.xml ]; then \
	  rm ParameterChanges/ParameterChanges.xml; \
	fi;

FORCE:

