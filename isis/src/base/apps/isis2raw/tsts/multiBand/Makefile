APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/isis2rawTruth6.raw \
	bittype= S16BIT > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/isis2rawTruth6NoNull.raw \
	null=no \
	bittype= S16BIT > /dev/null;
