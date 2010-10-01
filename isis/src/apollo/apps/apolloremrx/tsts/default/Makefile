APPNAME = apolloremrx

remrxTruth.txt.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/AS15-M-1450.warp.cub \
	to=$(OUTPUT)/AS15-M-1450.remrx.cub width=7 \
	action=patch resvalid=false > /dev/null;
	catlab from=$(OUTPUT)/AS15-M-1450.remrx.cub >& $(OUTPUT)/remrxTruth.txt;
