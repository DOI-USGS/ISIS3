APPNAME = slpmap

slpmapTruth.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth.cub output=percentslope > /dev/null;
	catlab from=$(OUTPUT)/slpmapTruth.cub > $(OUTPUT)/slpmapTruth.pvl;
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth2.cub pixres=user resolution=500 output=percentslope > /dev/null;\
	catlab from=$(OUTPUT)/slpmapTruth2.cub > $(OUTPUT)/slpmapTruth2.pvl;

