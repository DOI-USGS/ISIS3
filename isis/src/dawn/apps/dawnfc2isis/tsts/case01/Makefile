APPNAME = dawnfc2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/FC21B0001010_09049002212F5D.IMG \
         to=$(OUTPUT)/FC21B0001010_09049002212F5D.cub > /dev/null;
         catlab from=$(OUTPUT)/FC21B0001010_09049002212F5D.cub >& $(OUTPUT)/labels.txt;
