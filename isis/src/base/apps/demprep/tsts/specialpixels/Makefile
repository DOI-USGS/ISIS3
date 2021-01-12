APPNAME = demprep

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/vest64_dtm_specialpixels.cub \
	to=$(OUTPUT)/vest64_dtm_specialpixels.cub > /dev/null;
	catlab from=$(OUTPUT)/vest64_dtm_specialpixels.cub    \
	to=$(OUTPUT)/labels.txt > /dev/null;
