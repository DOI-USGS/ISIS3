APPNAME = apollofindrx

findrxTruth.pvl.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	cp $(INPUT)/AS15-M-1450.cub $(OUTPUT)/AS15-M-1450.findrx.cub;
	$(APPNAME) from=$(OUTPUT)/AS15-M-1450.findrx.cub > /dev/null;
	catlab from=$(OUTPUT)/AS15-M-1450.findrx.cub > $(OUTPUT)/findrxTruth.pvl 2>&1;
	rm $(OUTPUT)/AS15-M-1450.findrx.cub;
