APPNAME = cnetthinner
include $(ISISROOT)/make/isismake.tsts

# History comments 

commands:
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/tolSmall.net maxpoints=200 \
	   tolerance = 0.0 >& /dev/null; \
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/tolMedium.net maxpoints=200 \
	   tolerance = 0.5 >& /dev/null; \
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/tolLarge.net maxpoints=200 \
	   tolerance=1.0 >& /dev/null; \
	grep -a NumberOfPoints $(OUTPUT)/tolSmall.net >& $(OUTPUT)/sm.txt;  \
        grep -a NumberOfPoints $(OUTPUT)/tolMedium.net >& $(OUTPUT)/med.txt; \
        grep -a NumberOfPoints $(OUTPUT)/tolLarge.net >& $(OUTPUT)/large.txt;  
	 
