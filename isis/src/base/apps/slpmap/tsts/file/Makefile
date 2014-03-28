APPNAME = slpmap

slpmapTruth2.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/input.cub to=$(OUTPUT)/slpmapTruth.cub \
	  output=slope pixres=file > /dev/null;
