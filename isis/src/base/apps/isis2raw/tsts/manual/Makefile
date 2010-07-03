APPNAME = isis2raw

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth7.raw \
	bittype= U16BIT \
	stretch= manual \
	minimum=0 \
	maximum=65535 > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	to=$(OUTPUT)/isis2rawTruth7NoNull.raw \
	null=no \
	bittype= U16BIT \
	stretch= manual \
	minimum = 0 \
	maximum = 65535 > /dev/null;
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	to=$(OUTPUT)/isis2rawTruth7HrsHis.raw \
	null=no hrs=yes his=yes \
	bittype= U16BIT \
	stretch= manual \
	minimum = 0 \
	maximum = 65535 > /dev/null;
