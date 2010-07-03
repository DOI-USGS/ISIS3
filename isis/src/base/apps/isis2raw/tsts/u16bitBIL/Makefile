APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth3.raw \
	storageorder=bil \
	bittype= U16BIT > /dev/null;
