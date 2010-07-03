APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth3.raw \
	storageorder=bip \
	bittype= U16BIT > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	to=$(OUTPUT)/isis2rawTruth3OMinOMaxLis.raw \
	storageorder=bip omin=0 omax=15 lis=yes \
	bittype= U16BIT > /dev/null;
