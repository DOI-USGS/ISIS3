APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image_error.lis \
	to=$(OUTPUT)/pointlistErrorTruth.pvl \
	append=false coordtype=IMAGE > /dev/null;
