include $(ISISROOT)/make/isismake.macros

# ISIS DOCS MAKEFILE
#   Create tables of contents for the Object
#   documentation and kicks off lower makefiles
#   dls - deborah lee soltesz - 4/25/2002
#   history
#     dls 3/4/2005 - modified to use doxygen instead
#                    of IsisOML system


TOCDIR = $(ISISROOT)doc/Object
OBJECTASSETS = $(wildcard $(ISISROOT)/src/*/objs/*/assets/)

help:
	echo "Isis Object Documentation Make System Commands"
	echo "------------------------ "
	echo "make docs      : Build Isis Object Documentation"
	echo " "
	echo "make wwwdocsrc : Sync Source to Public Web Server" 
	echo "make devdocsrc : Sync Source to Development Web Server" 
	echo "make wwwdoc    : Sync Documentation to Public Web Server" 
	echo "make devdoc    : Sync Documentation to Development Web Server" 
	echo " "
	echo "make clean     : Clean up Source Area (docsys/Object)" 


docs: documentation assetsDir conf render

documentation: 
	echo "      Creating Object Documentation"
	mkdir -p $(ISISROOT)/doc/Object
	mkdir -p $(ISISROOT)/doc/Object/apps
	mkdir -p $(ISISROOT)/doc/Object/Developer
	mkdir -p $(ISISROOT)/doc/Object/Programmer
#dls 2006-10-04 -- took out -d on rsync that i'd added to make Sun happy
# (Sun = hipeaks)... Linux (orkin) was complaining
	rsync -qlHptr --delete --rsh=ssh *.html $(ISISROOT)/doc/Object/
	rsync -qlHptr --delete --rsh=ssh build/isisDoxyDefs.doxydef $(ISISROOT)/doc/documents/DocStyle/assets/


assetsDir:
	echo "        ...copying assets"
	rsync -qlHptr --delete --rsh=ssh assets $(ISISROOT)/doc/Object/ 

conf:
	echo "        ...building apps configuration"
	mkdir -p build/apps
	cat build/apps_tag.conf                                     >  build/apps_tag_temp.conf
	echo "OUTPUT_DIRECTORY = $(ISISROOT)/doc/Object/"           >> build/apps_tag_temp.conf
	echo "STRIP_FROM_PATH  = $(ISISROOT)/"                      >> build/apps_tag_temp.conf
	echo "INPUT            = $(ISISROOT)/src/ \\"               >> build/apps_tag_temp.conf
	echo "                   build/isisAppsDoxyDefs.doxydef"    >> build/apps_tag_temp.conf
	echo "HTML_HEADER      = build/IsisObjectHeaderApps.html"   >> build/apps_tag_temp.conf
	echo "HTML_FOOTER      = build/IsisObjectFooter.html"       >> build/apps_tag_temp.conf
	echo "HTML_OUTPUT      = apps"                              >> build/apps_tag_temp.conf
	echo "HTML_STYLESHEET  = build/doxygen_apps.css"            >> build/apps_tag_temp.conf

	echo "        ...building Programmer configuration"
	cat build/Programmer.conf                                        >  build/Programmer_temp.conf
	echo "OUTPUT_DIRECTORY = $(ISISROOT)/doc/Object/"                >> build/Programmer_temp.conf
	echo "FILE_PATTERNS    = *objs*.h \ "                            >> build/Programmer_temp.conf
	echo "                   *objs*.cpp \ "                          >> build/Programmer_temp.conf
	echo "                   *build/isisDoxyDefs.doxydef"            >> build/Programmer_temp.conf
	echo "STRIP_FROM_PATH  = $(ISISROOT)/"                           >> build/Programmer_temp.conf
	echo "INPUT            = build/isisDoxyDefs.doxydef $(ISISROOT)/src/ " >> build/Programmer_temp.conf
	echo "HTML_HEADER      = build/IsisObjectHeaderProgrammers.html" >> build/Programmer_temp.conf
	echo "HTML_FOOTER      = build/IsisObjectFooter.html"            >> build/Programmer_temp.conf
	echo "HTML_OUTPUT      = Programmer"                             >> build/Programmer_temp.conf
	echo "TAGFILES         = build/apps/apps_fix.tag"                >> build/Programmer_temp.conf
	echo "HTML_STYLESHEET  = build/doxygen_prog.css"                 >> build/Programmer_temp.conf
	echo "IMAGE_PATH       = "                                     >> build/Programmer_temp.conf

	if [ "$(findstring LOUD,$(MODE))" = "LOUD" ]; \
	then \
	  echo "QUIET                  = NO"                         >> build/Programmer_temp.conf; \
	  echo "WARNINGS               = YES"                        >> build/Programmer_temp.conf; \
	  echo "WARN_IF_UNDOCUMENTED   = YES"                        >> build/Programmer_temp.conf; \
	  echo "WARN_IF_DOC_ERROR      = YES"                        >> build/Programmer_temp.conf; \
	  echo "WARN_NO_PARAMDOC       = YES"                        >> build/Programmer_temp.conf; \
        else \
	  echo "QUIET                  = YES"                        >> build/Programmer_temp.conf; \
	  echo "WARNINGS               = YES"                        >> build/Programmer_temp.conf; \
	  echo "WARN_IF_UNDOCUMENTED   = YES"                        >> build/Programmer_temp.conf; \
	  echo "WARN_IF_DOC_ERROR      = YES"                        >> build/Programmer_temp.conf; \
	  echo "WARN_NO_PARAMDOC       = YES"                        >> build/Programmer_temp.conf; \
	fi

	for dirname in "$(OBJECTASSETS)"; do \
	  echo "$$dirname \\"                                          >> build/Programmer_temp.conf ; \
	done

	echo "        ...building Developer configuration"
	cat build/Developer.conf                                    >  build/Developer_temp.conf
	echo "OUTPUT_DIRECTORY = $(ISISROOT)/doc/Object/"           >> build/Developer_temp.conf
	echo "STRIP_FROM_PATH  = $(ISISROOT)/"                      >> build/Developer_temp.conf
	echo "INPUT            = $(ISISROOT)/src/ \\"               >> build/Developer_temp.conf
	echo "                   build/isisDoxyDefs.doxydef"        >> build/Developer_temp.conf
	echo "HTML_HEADER      = build/IsisObjectHeader.html"       >> build/Developer_temp.conf
	echo "HTML_FOOTER      = build/IsisObjectFooter.html"       >> build/Developer_temp.conf
	echo "HTML_OUTPUT      = Developer"                         >> build/Developer_temp.conf
	echo "TAGFILES         = build/apps/apps_fix.tag"           >> build/Developer_temp.conf
	echo "HTML_STYLESHEET  = build/doxygen.css"                 >> build/Developer_temp.conf
	echo "IMAGE_PATH       = "                                >> build/Developer_temp.conf
	if [ "$(findstring LOUD,$(MODE))" = "LOUD" ]; \
	then \
	  echo "QUIET                  = NO"                         >> build/Developer_temp.conf; \
	  echo "WARNINGS               = YES"                        >> build/Developer_temp.conf; \
	  echo "WARN_IF_UNDOCUMENTED   = YES"                        >> build/Developer_temp.conf; \
	  echo "WARN_IF_DOC_ERROR      = YES"                        >> build/Developer_temp.conf; \
	  echo "WARN_NO_PARAMDOC       = YES"                        >> build/Developer_temp.conf; \
        else \
	  echo "QUIET                  = YES"                        >> build/Developer_temp.conf; \
	  echo "WARNINGS               = NO"                         >> build/Developer_temp.conf; \
	  echo "WARN_IF_UNDOCUMENTED   = NO"                         >> build/Developer_temp.conf; \
	  echo "WARN_IF_DOC_ERROR      = NO"                         >> build/Developer_temp.conf; \
	  echo "WARN_NO_PARAMDOC       = NO"                         >> build/Developer_temp.conf; \
	fi

	for dirname in "$(OBJECTASSETS)" ; do \
	    echo "$$dirname \\"                                     >> build/Developer_temp.conf ; \
	done


render: doApps doProg doDev
doApps:
	echo "        ...building apps documentation"
	doxygen build/apps_tag_temp.conf
#	cp build/apps/apps.tag $(ISISROOT)/doc/Object/apps/
	$(XALAN) -o build/apps/apps_fix.tag build/apps/apps.tag build/IsisApplicationTagFix.xsl
	cp build/apps/apps_fix.tag $(ISISROOT)/doc/Object/apps/

doProg:
	echo "        ...building Programmer documentation"
	doxygen build/Programmer_temp.conf
	cd $(ISISROOT)/doc/Object/Programmer ; perl installdox -l apps_fix.tag@../apps/ -q ; cd $(ISISROOT)/src/docsys/Object/
	
doDev:
	echo "        ...building Developer documentation"
	doxygen build/Developer_temp.conf
	cd $(ISISROOT)/doc/Object/Developer ; perl installdox -l apps_fix.tag@../apps/ -q ; cd $(ISISROOT)/src/docsys/Object/

doProgTest:
	echo "Building ProgTester configuration"
	mkdir -p docbuild
	cat $(ISISROOT)/src/docsys/Object/build//Programmer.conf         >  docbuild/ProgTester_temp.conf
	echo "FILE_PATTERNS          = *.h \ "                           >> docbuild/ProgTester_temp.conf
	echo "                         *.cpp \ "                         >> docbuild/ProgTester_temp.conf
	echo "                         *build/isisDoxyDefs.doxydef"      >> docbuild/ProgTester_temp.conf
	echo "OUTPUT_DIRECTORY = $(CURDIR)/"                             >> docbuild/ProgTester_temp.conf
	echo "STRIP_FROM_PATH  = $(CURDIR)/"                             >> docbuild/ProgTester_temp.conf
	echo "INPUT            = $(CURDIR)/ \\"                          >> docbuild/ProgTester_temp.conf
	echo "                   $(ISISROOT)/src/docsys/Object/build/isisDoxyDefs.doxydef"            >> docbuild/ProgTester_temp.conf
	echo "HTML_HEADER      = $(ISISROOT)/src/docsys/Object/build/IsisObjectHeaderProgTester.html" >> docbuild/ProgTester_temp.conf
	echo "HTML_FOOTER      = $(ISISROOT)/src/docsys/Object/build/IsisObjectFooterProgTester.html" >> docbuild/ProgTester_temp.conf
	echo "HTML_OUTPUT      = html"                                   >> docbuild/ProgTester_temp.conf
	echo "TAGFILES         = "                                       >> docbuild/ProgTester_temp.conf
	echo "HTML_STYLESHEET  = $(ISISROOT)/src/docsys/Object/build/doxygen.css"                     >> docbuild/ProgTester_temp.conf
	if [ "$(findstring LOUD,$(MODE))" = "LOUD" ]; \
	then \
	  echo "QUIET                  = NO"                           >> build/ProgTester_temp.conf; \
	  echo "WARNINGS               = YES"                          >> build/ProgTester_temp.conf; \
	  echo "WARN_IF_UNDOCUMENTED   = YES"                          >> build/ProgTester_temp.conf; \
	  echo "WARN_IF_DOC_ERROR      = YES"                          >> build/ProgTester_temp.conf; \
	  echo "WARN_NO_PARAMDOC       = YES"                          >> build/ProgTester_temp.conf; \
	else \
	  echo "QUIET                  = YES"                          >> build/ProgTester_temp.conf; \
	  echo "WARNINGS               = NO"                           >> build/ProgTester_temp.conf; \
	  echo "WARN_IF_UNDOCUMENTED   = NO"                           >> build/ProgTester_temp.conf; \
	  echo "WARN_IF_DOC_ERROR      = NO"                           >> build/ProgTester_temp.conf; \
	  echo "WARN_NO_PARAMDOC       = NO"                           >> build/ProgTester_temp.conf; \
	fi
	doxygen docbuild/ProgTester_temp.conf
	echo "View your documentation by opening the file html/index.html in a browser"
#do the following on a clean, as well as whacking the html directory created in this dir by doxy
	rm -r docbuild
	


#----
# CLEAN
clean:
	rm -f  build/apps_tag_temp.conf 
	rm -f  build/Programmer_temp.conf
	rm -f  build/Developer_temp.conf 
	rm -rf build/apps
	rm -f ../documents/DocStyle/assets/isisDoxyDefs.doxydef

cleandoc:
	rm -r $(ISISROOT)/doc/Object






#TODO
# the following are temprary macros for updating the websites with the latest document build.
# these need to be removed and replaced with a more reasonable update script

# public docsrc upload
wwwdocsrc:
	echo " "
	echo "_______________________________________________________________________________"
	echo "BEGIN SYNC: docsys to public server"
	rsync -vlHptr --delete --progress --rsh=ssh $(ISISROOT)/src/docsys/* webmaster@wwwflag:/www/httpd/Astrogeology/Extranet/htdocs/Projects/IsisXML/docsrc/

# dev docsrc upload
devdocsrc:
	echo " "
	echo "_______________________________________________________________________________"
	echo "BEGIN SYNC: docsys to dev server"
	rsync -vlHptr --delete --progress --rsh=ssh $(ISISROOT)/src/docsys/* dsoltesz@wwwdev:/www/httpd/Astrogeology/Extranet/htdocs/Projects/IsisXML/docsrc/

# public doc upload
wwwdoc:
	echo " "
	echo "_______________________________________________________________________________"
	echo "BEGIN SYNC: docs to public server"
	rsync -vlHptr --delete --progress --rsh=ssh $(ISISROOT)/doc/* webmaster@wwwflag:/www/httpd/Astrogeology/Extranet/htdocs/Projects/IsisXML/doc/

# dev doc upload
devdoc:
	echo " "
	echo "_______________________________________________________________________________"
	echo "BEGIN SYNC: docs to dev server"
	rsync -vlHptr --delete --progress --rsh=ssh $(ISISROOT)/doc/* dsoltesz@wwwdev:/www/httpd/Astrogeology/Extranet/htdocs/Projects/IsisXML/doc/

