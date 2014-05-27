APPNAME = slpmap

slpmapTruth2.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth.cub output=slope > /dev/null;
	catlab from=$(OUTPUT)/slpmapTruth.cub to=$(OUTPUT)/slpmapTruth.pvl > /dev/null;
	$(APPNAME) from=$(INPUT)/test.cub to=$(OUTPUT)/slpmapTruth2.cub output=slope units=radians > /dev/null;
	catlab from=$(OUTPUT)/slpmapTruth2.cub to=$(OUTPUT)/slpmapTruth2.pvl > /dev/null;

