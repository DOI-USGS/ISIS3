APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth5.raw \
	storageorder=bil \
	bittype= 8BIT \
	stretch= none > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth5NoNullLisHrs.raw \
	storageorder=bil \
	bittype= 8BIT null=no lis=yes hrs=yes \
	stretch= none > /dev/null;
