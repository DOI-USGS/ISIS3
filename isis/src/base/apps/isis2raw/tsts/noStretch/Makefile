APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth5.raw \
	bittype= 8BIT \
	stretch= none > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth5NoNull.raw \
	null=no \
	bittype= 8BIT \
	stretch= none > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	to=$(OUTPUT)/isis2rawTruth5AllAndOMinOMax.raw \
	null=no lrs=yes lis=yes his=yes hrs=yes \
	omin=1 omax=200 \
	bittype= 8BIT \
	stretch= none > /dev/null;
