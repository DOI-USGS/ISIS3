APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	to=$(OUTPUT)/isis2rawTruth2.raw \
	storageorder=bil \
	bittype= S16BIT \
	endian= MSB > /dev/null;
