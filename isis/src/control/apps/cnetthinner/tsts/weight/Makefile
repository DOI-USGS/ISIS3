APPNAME = cnetthinner
include $(ISISROOT)/make/isismake.tsts

# History comments 
# Right now, these will all be identical...

commands:
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/Small.net maxpoints=200 \
	   weight = 0.0 >& /dev/null;
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/Medium.net maxpoints=200 \
	   weight = 0.5 >& /dev/null;
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/Large.net maxpoints=200 \
	   weight = 1.0 >& /dev/null;
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/XLarge.net maxpoints=200 \
	   weight = 10000.0 >& /dev/null;
	grep -a NumberOfPoints $(OUTPUT)/Small.net >& $(OUTPUT)/sm.txt;  \
        grep -a NumberOfPoints $(OUTPUT)/Medium.net >& $(OUTPUT)/med.txt; \
        grep -a NumberOfPoints $(OUTPUT)/Large.net >& $(OUTPUT)/large.txt; \
        grep -a NumberOfPoints $(OUTPUT)/XLarge.net >& $(OUTPUT)/Xlarge.txt;  
	 
