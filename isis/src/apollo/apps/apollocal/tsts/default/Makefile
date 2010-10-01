APPNAME = apollocal

calTruth.txt.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/AS15-M-1450.remrx.cub \
	to=$(OUTPUT)/AS15-M-1450.cal.cub > /dev/null;
	catlab from=$(OUTPUT)/AS15-M-1450.cal.cub >& $(OUTPUT)/calTruth.txt;
