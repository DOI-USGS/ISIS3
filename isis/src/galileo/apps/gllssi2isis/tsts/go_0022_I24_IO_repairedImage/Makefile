APPNAME = gllssi2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/8500s.lbl \
	  to=$(OUTPUT)/8500s.cub > /dev/null;
	  catlab from=$(OUTPUT)/8500s.cub >& $(OUTPUT)/labels.txt;
