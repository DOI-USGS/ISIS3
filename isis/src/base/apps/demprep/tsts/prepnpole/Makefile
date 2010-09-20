APPNAME = demprep

labels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ulcn2005_lpo_npole.cub \
	  to=$(OUTPUT)/ulcn2005_lpo_npole_pad.cub > /dev/null;
	  catlab from=$(OUTPUT)/ulcn2005_lpo_npole_pad.cub >& $(OUTPUT)/labels.txt;
