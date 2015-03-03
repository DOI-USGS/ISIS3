APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/pointlistNoAppendTruth.pvl \
	coordtype=IMAGE append=FALSE > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/pointlistNoAppendTruth.pvl \
	coordtype=IMAGE append=FALSE > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	format=FLAT coordtype=IMAGE \
	to=$(OUTPUT)/pointlistNoAppendTruth.csv \
	append=FALSE > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	format=FLAT coordtype=IMAGE \
	to=$(OUTPUT)/pointlistNoAppendTruth.csv \
	append=FALSE > /dev/null;
