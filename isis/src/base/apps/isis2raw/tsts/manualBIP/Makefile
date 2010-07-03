APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth7.raw \
	storageorder=bip \
	bittype= U16BIT \
	stretch= manual \
	minimum = 0 \
	maximum = 65535 > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth7All.raw \
	storageorder=bip \
	bittype= U16BIT \
	lrs=yes lis=yes his=yes hrs=yes \
	stretch= manual \
	minimum = 0 \
	maximum = 65535 > /dev/null;
