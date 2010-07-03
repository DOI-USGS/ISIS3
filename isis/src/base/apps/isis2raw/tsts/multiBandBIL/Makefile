APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/isis2rawTruth6.raw \
	storageorder=bil \
	bittype= S16BIT > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/isis2rawTruth6LrsHis.raw \
	storageorder=bil \
	lrs=yes his=yes \
	bittype= S16BIT > /dev/null;
