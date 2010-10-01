APPNAME = apollowarp

warpTruth.txt.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/AS15-M-1450.findrx.cub \
	to=$(OUTPUT)/AS15-M-1450.warp.cub degree=3 \
	weighted=true interpolation=cubicconvolution > /dev/null;
	catlab from=$(OUTPUT)/AS15-M-1450.warp.cub >& $(OUTPUT)/warpTruth.txt;
