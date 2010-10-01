APPNAME = apollo2isis

isisTruth.txt.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/AS15-M-1450.lbl \
	to=$(OUTPUT)/AS15-M-1450.cub > /dev/null;
	catlab from=$(OUTPUT)/AS15-M-1450.cub >& $(OUTPUT)/isisTruth.txt;
