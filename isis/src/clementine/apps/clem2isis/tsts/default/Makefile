APPNAME = clem2isis

nirLabels.txt.IGNORELINES = Bytes StartByte ByteOrder
uvvisLabels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/lua1855j.310 \
         to=$(OUTPUT)/lua1855j.truth.cub > /dev/null;
	 $(RM) paramtrs.dat;
	 catlab from=$(OUTPUT)/lua1855j.truth.cub >& $(OUTPUT)/uvvisLabels.txt;
	 $(APPNAME) from=$(INPUT)/lna3056k.284 \
         to=$(OUTPUT)/lna3056k.truth.cub > /dev/null;
	 $(RM) paramtrs.dat;
	 catlab from=$(OUTPUT)/lna3056k.truth.cub >& $(OUTPUT)/nirLabels.txt;
