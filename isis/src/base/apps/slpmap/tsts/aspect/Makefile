APPNAME = slpmap

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth.cub output=aspect > /dev/null;
	catlab from=$(OUTPUT)/slpmapTruth.cub > $(OUTPUT)/slpmapTruth.pvl;
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth2.cub output=aspect units=radians > /dev/null;
	catlab from=$(OUTPUT)/slpmapTruth2.cub > $(OUTPUT)/slpmapTruth2.pvl;

