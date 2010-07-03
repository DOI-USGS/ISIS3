APPNAME = raw2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isis2rawTruth3.raw to=$(OUTPUT)/raw2isisTruth.cub \
	samples=126 lines=126 bittype=unsignedword \
	> /dev/null;
