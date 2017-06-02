APPNAME = cnetthinner

include $(ISISROOT)/make/isismake.tsts

# History comments 

commands:
	#MAXPOINTS
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/400.net \
	  maxpoints=400 >& /dev/null; \
        $(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/200.net \
	  maxpoints=200 >& /dev/null; \
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/50.net \
	  maxpoints=50 >& /dev/null; \
	grep -a NumberOfPoints $(OUTPUT)/50.net >& $(OUTPUT)/50.txt;  \
        grep -a NumberOfPoints $(OUTPUT)/200.net >& $(OUTPUT)/200.txt; \
        grep -a NumberOfPoints $(OUTPUT)/400.net >& $(OUTPUT)/400.txt; 

	#MINPOINTS

