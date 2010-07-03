APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth3.raw \
	bittype= U16BIT > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth3NoNull.raw \
	null=no \
	bittype= U16BIT > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth3NoNullOMax.raw \
	null=no omax=12000 \
	bittype= U16BIT > /dev/null;
