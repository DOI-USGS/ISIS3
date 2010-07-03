APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/isis2rawTruth6.raw \
	storageorder=bip \
	bittype= S16BIT > /dev/null;
