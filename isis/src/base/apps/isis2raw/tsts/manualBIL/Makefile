APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth7.raw \
	storageorder=bil \
	bittype= U16BIT \
	stretch= manual \
	minimum = 0 \
	maximum = 65535 > /dev/null;
