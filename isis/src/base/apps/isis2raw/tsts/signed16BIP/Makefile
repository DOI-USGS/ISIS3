APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	to=$(OUTPUT)/isis2rawTruth2.raw \
	storageorder=bip \
	bittype= S16BIT \
	endian= MSB > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	to=$(OUTPUT)/isis2rawTruth2OMinNoNullHrs.raw \
	storageorder=bip \
	bittype= S16BIT \
	null=no hrs=yes omin=-5000 \
	endian= MSB > /dev/null;
