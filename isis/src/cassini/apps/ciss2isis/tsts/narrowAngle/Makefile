APPNAME = ciss2isis

labels.txt.IGNORELINES = Bytes StartByte

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/N1472853667_1.LBL \
	to=$(OUTPUT)/N1472853667_1.truth.cub > /dev/null;
	catlab from=$(OUTPUT)/N1472853667_1.truth.cub >& $(OUTPUT)/labels.txt;
