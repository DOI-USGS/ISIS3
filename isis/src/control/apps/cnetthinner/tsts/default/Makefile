APPNAME = cnetthinner

include $(ISISROOT)/make/isismake.tsts

# History comments 

commands:
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/output.net maxpoints=20000 \
	   suppressed=ignore >& /dev/null;
	$(APPNAME) cnet=$(INPUT)/customPointsTruth.net onet=$(OUTPUT)/thinned.net maxpoints=20000 \
	   suppressed=ignore networkid=testID >& /dev/null;

