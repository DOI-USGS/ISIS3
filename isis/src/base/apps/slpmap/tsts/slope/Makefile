APPNAME = slpmap

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth.cub output=slope > /dev/null;
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth2.cub output=slope units=radians > /dev/null;
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth3.cub pixres=user resolution=500 > /dev/null;
