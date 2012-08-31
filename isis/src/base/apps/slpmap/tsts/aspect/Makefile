APPNAME = slpmap

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth.cub output=aspect > /dev/null;
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth2.cub output=aspect units=radians > /dev/null;
