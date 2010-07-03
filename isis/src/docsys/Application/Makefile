include $(ISISROOT)/make/isismake.macros

# ISIS DOCS MAKEFILE
#   Create tables of contents for the Application
#   documentation and kicks off lower makefiles
#   dls - deborah lee soltesz - 4/25/2002


TOCDIR = $(ISISROOT)/doc/Application


help:
	echo "Isis Application Documentation Make System Commands"
	echo "------------------------ "
	echo "make docs      : Build Isis Application Documentation"
	echo " "
	echo "make wwwdocsrc : Sync Source to Public Web Server"
	echo "make devdocsrc : Sync Source to Development Web Server"
	echo "make wwwdoc    : Sync Documentation to Public Web Server"
	echo "make devdoc    : Sync Documentation to Development Web Server"
	echo " "
	echo "make clean     : Clean up Source Area (docsys/Application)"



#---------------
# DOCS: generate documentation

docs: documentation toccat tocoldvnew tocalph categories


# APPLICATION LIST
# build the TOC XML for all the applications

documentation:
	echo "      Creating Documentation"
	mkdir -p $(ISISROOT)/doc/Application
	cd data ; make docs ; cd .. ;


#---------------
# TABLES OF CONTENTS
# build tables of contents to doc directory

tocalph: $(TOCDIR)/alpha.html
$(TOCDIR)/alpha.html: build/toc.xml build/TOCindex_alpha.xsl
	echo "      Creating Alphabetical Applications List"
	$(XALAN) -p menuPath "'../'" -o $(TOCDIR)/alpha.html build/toc.xml build/TOCindex_alpha.xsl

toccat: $(TOCDIR)/index.html
$(TOCDIR)/index.html: build/toc.xml build/TOCindex_category.xsl
	echo "      Creating Categorical Applications List"
	$(XALAN) -p menuPath "'../'" -o $(TOCDIR)/index.html build/toc.xml build/TOCindex_category.xsl

tocoldvnew: $(TOCDIR)/oldvnew.html
$(TOCDIR)/oldvnew.html: build/toc.xml build/TOCindex_oldvnew.xsl
	echo "      Creating Old vs. New Applications List"
	$(XALAN) -p menuPath "'../'" -o $(TOCDIR)/oldvnew.html build/toc.xml build/TOCindex_oldvnew.xsl

categories: ../Schemas/Application/application.xsd build/IsisApplicationCategoriesbuild.xsl
	echo "      Generating list of categories from the AML Schema"
	mkdir -p $(ISISROOT)/bin/xml/
	$(XALAN) -o $(ISISROOT)/bin/xml/applicationCategories.xml ../Schemas/Application/application.xsd build/IsisApplicationCategoriesbuild.xsl
	


#---------------
# CLEAN: clean up extraneous files

clean:
	rm -f build/toc.xml 
	cd data	; make clean ; cd .. ;


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

