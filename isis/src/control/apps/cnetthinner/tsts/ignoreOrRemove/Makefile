APPNAME = cnetthinner
APPNAME2 = cnetedit 
APPNAME3 = cnetdiff

include $(ISISROOT)/make/isismake.tsts

# History comments 

commands:
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/ignored.net maxpoints=200 \
	   suppressed=ignore >& /dev/null;
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/removed.net maxpoints=200 \
	   suppressed=remove >& /dev/null;
	$(APPNAME2) cnet=$(OUTPUT)/ignored.net onet=$(OUTPUT)/ignored_removed.net \
	  delete=yes >& /dev/null;  
	$(APPNAME3) from=$(OUTPUT)/ignored_removed.net from2=$(OUTPUT)/removed.net report=full \
	  >& $(OUTPUT)/cnetdiff.txt; \
        $(RM) $(OUTPUT)/ignored_removed.net >& /dev/null; 
	 
	 
