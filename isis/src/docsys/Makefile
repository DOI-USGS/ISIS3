include $(ISISROOT)/make/isismake.macros

# ISIS DOCS MAKEFILE
#   Move assets and static documents to doc root
#   and kick off lower makefiles
#   dls - deborah lee soltesz - 4/25/2002



#APPLICATIONS = $(filter-out Makefile CVS $(wildcard *.*) , data/$(wildcard *))
#DOCUMENTS    = $(filter-out Makefile $(wildcard *.*) , documents/$(wildcard *))
DOCDIR = $(ISISROOT)/doc
USER = webman
USERDEV = isis3mgr

help:
	echo "Isis Documentation Make System Commands"
	echo "------------------------ "
	echo "make docs      : Build Isis Web Documentation"
	echo " "
	echo "make schemas   : Update Isis XML Schemas"
	echo "make assetsdir : Update assets"
	echo " "
	echo "make wwwdoc    : Sync Documentation to Public Web Server" 
	echo "make devdoc    : Sync Documentation to Development Web Server" 
	echo "make wwwschema : Sync Schemas to Public Web Server" 
	echo "make syncall   : Sync Everything to All Web Servers" 
	echo "make syncdev   : Sync Everything to Development" 
	echo " "
	echo "make clean     : Clean up Source Area (docsys)" 

docs: upperlevel

upperlevel:

	echo " "
	echo "    Building Upper Level Documentation"
	echo "      Updating Directories"

	echo "        [assets]"
	rsync -lHptr --delete --rsh=ssh --cvs-exclude assets $(DOCDIR)

	echo "        [P3P]"
	rsync -lHptr --delete --rsh=ssh --cvs-exclude w3c $(DOCDIR)

	echo "        [UserDocs]"
	mkdir -p UserDocs
	rsync -lHptr --delete --rsh=ssh --cvs-exclude UserDocs $(DOCDIR)

	echo "        [AboutIsis]"
	mkdir -p AboutIsis
	rsync -lHptr --delete --rsh=ssh --cvs-exclude AboutIsis $(DOCDIR)

	echo "        [General]"
	mkdir -p General
	rsync -lHptr --delete --rsh=ssh --cvs-exclude General $(DOCDIR)

	echo "        [Guides]"
	mkdir -p Guides
	rsync -lHptr --delete --rsh=ssh --cvs-exclude Guides $(DOCDIR)

	echo "        [Installation]"
	mkdir -p Installation
	rsync -lHptr --delete --rsh=ssh --cvs-exclude Installation $(DOCDIR)

	echo "        [TechnicalInfo]"
	mkdir -p TechnicalInfo
	rsync -lHptr --delete --rsh=ssh --cvs-exclude TechnicalInfo $(DOCDIR)

	echo "        [Schemas]"
	mkdir -p Schemas
	rsync -lHptr --delete --rsh=ssh --cvs-exclude Schemas $(DOCDIR)

	echo "        [home page]"
	#rsync -lHptr --delete --rsh=ssh --cvs-exclude *.html $(DOCDIR)/
	$(XALAN) -v -p menuPath "'./'" -o $(DOCDIR)/index.html build/homepage.xml build/main.xsl

	echo "      Finished Updating Directories"

	echo " "
	echo "      Building Documents"
	mkdir -p $(DOCDIR)/documents
	cd documents ; make docs ; cd .. ;
	echo "      Finished Building Documents"

	echo "    Finished Building Upper Level Documentation"
	echo " "


	echo "    Building Application Documentation"
	cd Application ; make docs ; cd .. ;
	echo "    Finished Building Application Documentation"
	echo " "

	echo "    Building Object Documentation"
	cd Object ; make docs ; cd .. ;
	echo "    Finished Building Object Documentation"
	echo " "

	chmod -R ugo+r $(DOCDIR)



assetsdir: 
	echo " "
	echo "    Updating assets"
	rsync -lHptr --delete --rsh=ssh --cvs-exclude assets $(DOCDIR)
	echo "    Finished Updating assets"
	echo " "

schemas:
	echo " "
	echo "    Updating Isis XML Schemas"
	rsync -lHptr --delete --rsh=ssh --cvs-exclude Schemas $(DOCDIR)/
	echo "    Finished Updating Isis XML Schemas"
	echo " "


clean:
	rm -rf UserDocs
	rm -rf AboutIsis
	rm -rf General      
	rm -rf Guides       
	rm -rf Installation 
	rm -rf TechnicalInfo
	rm -rf cgi-bin

	cd Schemas        && rm -f *.html     ; cd .. ;
	cd build          && rm -f doctoc.xml ; cd .. ;

	cd Application ; make clean ; cd .. ;
	cd Object ; make clean ; cd .. ;
	cd documents ; make clean ; cd .. ;

cleandocs:
	rm -r $(DOCDIR)/*

#TODO
# the following are temprary macros for updating the websites with the latest document build.
# these need to be removed and replaced with a more reasonable update script


# public doc upload
wwwdoc: wwwschema
	echo " "
	echo "_______________________________________________________________________________"
	echo "BEGIN SYNC: docs to public server"
	rsync -vlHtr --delete --progress --rsh=ssh --exclude 'Schemas/*' --cvs-exclude $(ISISROOT)/doc/ $(USER)@astroweb:/www/sites/Isis/htdocs/

devdoc:  
	echo " "
	echo "_______________________________________________________________________________"
	echo "BEGIN SYNC: docs to development server"
	rsync -vlHtr --delete --progress --rsh=ssh --cvs-exclude $(ISISROOT)/doc/ $(USERDEV)@astrodev:/www/sites/Isis/htdocs/

# copy Schemas to public www

wwwschema: schemas
	echo " "
	echo "_______________________________________________________________________________"
	echo "SYNC: schemas to public server"
	rsync -vlHtr --progress --rsh=ssh --cvs-exclude $(ISISROOT)/doc/Schemas/ $(USER)@astroweb:/www/sites/Isis/htdocs/Schemas/
	rsync -vlHptr --delete --progress --rsh=ssh --cvs-exclude $(ISISROOT)/doc/assets $(USER)@astroweb:/www/sites/Isis/htdocs/



syncall:
	echo " "
	echo " "
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	echo "~                                  SYNC ALL                                   ~"
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	make wwwdoc
	make devdoc
	
syncdev:
	echo " "
	echo " "
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	echo "~                                  SYNC DEV                                   ~"
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	make devdoc

