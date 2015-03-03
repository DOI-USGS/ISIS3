APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/pointlistAppendTruth.pvl \
	coordtype=IMAGE > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/pointlistAppendTruth.pvl \
	coordtype=IMAGE > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	format=FLAT coordtype=IMAGE \
	to=$(OUTPUT)/pointlistAppendTruth.csv > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	format=FLAT coordtype=IMAGE \
	to=$(OUTPUT)/pointlistAppendTruth.csv > /dev/null;
