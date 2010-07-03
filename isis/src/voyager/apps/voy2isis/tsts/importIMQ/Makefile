APPNAME = voy2isis

c1856306.labels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/c1856306.imq \
	to=$(OUTPUT)/c1856306.truth.cub > /dev/null;
	catlab from=$(OUTPUT)/c1856306.truth.cub >& $(OUTPUT)/c1856306.labels.pvl;
