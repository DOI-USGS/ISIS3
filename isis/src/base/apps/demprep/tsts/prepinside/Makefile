APPNAME = demprep

labels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ulcn2005_lpo_inside.cub \
	  to=$(OUTPUT)/ulcn2005_lpo_inside_pad.cub > /dev/null;
	  catlab from=$(OUTPUT)/ulcn2005_lpo_inside_pad.cub to=$(OUTPUT)/labels.txt > /dev/null;
