APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth4.raw \
	storageorder=bil \
	bittype= 8BIT > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	to=$(OUTPUT)/isis2rawTruth4MinMax.raw \
	storageorder=bil bittype= 8BIT \
	omin=40 omax=125 > /dev/null;
